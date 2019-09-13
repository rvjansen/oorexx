/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                                         */
/*                                                                            */
/* Redistribution and use in source and binary forms, with or                 */
/* without modification, are permitted provided that the following            */
/* conditions are met:                                                        */
/*                                                                            */
/* Redistributions of source code must retain the above copyright             */
/* notice, this list of conditions and the following disclaimer.              */
/* Redistributions in binary form must reproduce the above copyright          */
/* notice, this list of conditions and the following disclaimer in            */
/* the documentation and/or other materials provided with the distribution.   */
/*                                                                            */
/* Neither the name of Rexx Language Association nor the names                */
/* of its contributors may be used to endorse or promote products             */
/* derived from this software without specific prior written permission.      */
/*                                                                            */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "LocalQueueManager.hpp"
#include "Encodings.hpp"
#include "LocalAPIManager.hpp"
#include "SysLocalAPIManager.hpp"
#include "rexx.h"
#include "ClientMessage.hpp"
#include "Utilities.hpp"
#include <ctype.h>

// make sure we remember what we do for this process.
bool LocalQueueManager::createdSessionQueue = false;

/**
 * Initialize the local queue manager.
 */
LocalQueueManager::LocalQueueManager() : LocalAPISubsystem()
{
    localManager = NULL;
    sessionQueue = 0;
    sessionID = 0;
}

/**
 * Validate a queue name
 *
 * @param username  The name to validate.
 * @param isSession A bool to return to indicate the queue name is the session queue.
 *
 * @return True if the name is valid, false if invalid or "SESSION"
 */
bool LocalQueueManager::validateQueueName(const char *username, bool &isSession)
{
    isSession = false;

    if (username == NULL)               /* NULL is OK.                */
    {
        return true;
    }
    // "SESSION" is a reserved name, reject this in this context
    if (Utilities::strCaselessCompare(username, "SESSION") == 0)
    {
        // indicate to the caller this is the session queue
        isSession = true;
        return false;
    }

    size_t namelen = strlen(username);
    if (namelen > 0 && namelen <= MAX_QUEUE_NAME_LENGTH)
    {
        const char *valptr = username;      /* point to name              */
        char ch;
        while ((ch = *(valptr++)))
        {         /* While have not reached end */
            ch = toupper(ch);               /* convert to upper case      */
            if (!isalpha(ch) && !isdigit(ch) && ch != ch_PERIOD &&
                ch != ch_QUESTION_MARK && ch != ch_EXCLAMATION && ch != ch_UNDERSCORE)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * Initialize the local (client) queue manager instance.
 *
 * @param a      The local API manager instance.
 */
void LocalQueueManager::initializeLocal(LocalAPIManager *a)
{
    localManager = a;
    LocalAPISubsystem::initializeLocal(a);
    // find the session queue
    sessionQueue = initializeSessionQueue(a->getSession());
}


/**
 * Handle process termination.
 */
void LocalQueueManager::terminateProcess()
{
    // if we have a session queue
    if (sessionQueue != 0)
    {
        try
        {
            deleteSessionQueue();    // try to delete this
        }
        catch (ServiceException *e)
        {
            // just ignore any errors here.
            delete e;
        }
        // clear this out.
        sessionQueue = 0;
    }
                                 // continue the shutdown
    LocalAPISubsystem::terminateProcess();
}


/**
 * Create the session queue for this process.
 *
 * @param session
 *
 * @return
 */
QueueHandle LocalQueueManager::initializeSessionQueue(SessionID session)
{
    // first check to see if we have an env variable set...if we do we
    // inherit from our parent session
    QueueHandle mysessionQueue;
    // we could be inheriting the session queue from a caller process...check first.
    if (!createdSessionQueue && SysLocalAPIManager::getActiveSessionQueue(mysessionQueue))
    {
        // make sure we update the nest count
        // this might end up creating a new queue instance
        mysessionQueue = nestSessionQueue(session, mysessionQueue);
    }
    else
    {
        // create a new session queue
        mysessionQueue = createSessionQueue(session);
        // remember that we created this initially.  We'll need to create
        // the queue each time one is needed for this session.
        createdSessionQueue = true;
    }
    SysLocalAPIManager::setActiveSessionQueue(mysessionQueue);
    return mysessionQueue;
}

/**
 * Create a new session queue
 *
 * @param session The session identifier
 *
 * @return The handle of the session queue.
 */
QueueHandle LocalQueueManager::createSessionQueue(SessionID session)
{
    ClientMessage message(QueueManager, CREATE_SESSION_QUEUE, session);
    message.send();
    // the handle is returned in the first parameter
    return (QueueHandle)message.parameter1;
}


/**
 * Create a named queue
 *
 * @param name   The requested queue name.
 * @param size   The size of the buffer for the returned name.
 * @param createdName
 *               The buffer for the returned name.
 * @param dup    The duplicate flag.
 *
 * @return true if the requested name already exists and a new name
 *         was provided.
 */
RexxReturnCode LocalQueueManager::createNamedQueue(const char *name, size_t size, char *createdName, size_t *dup)
{
    // validation and copying of the request name only is necessary if given.
    if (name != NULL)
    {
        bool isSession = false;
        // make sure this is a valid name (session is not valid here)
        if (!validateQueueName(name, isSession))
        {
            return RXQUEUE_BADQNAME;
        }

        ClientMessage message(QueueManager, CREATE_NAMED_QUEUE, name);
        // the session id is used for creating unique queue names
        message.parameter1 = localManager->getSession();

        message.send();
        strncpy(createdName, message.nameArg, size);
        // return the dup name indicator
        *dup = message.result == DUPLICATE_QUEUE_NAME;
        // everything worked here.
        return RXQUEUE_OK;
    }
    else
    {
        // always returning a generated name
        ClientMessage message(QueueManager, CREATE_NAMED_QUEUE, "");
        // the session id is used for creating unique queue names
        message.parameter1 = localManager->getSession();

        message.send();
        strncpy(createdName, message.nameArg, size);
        // by definition, this is not a duplicate
        *dup = false;
        // everything worked here.
        return RXQUEUE_OK;
    }
}


/**
 * Create a named queue
 *
 * @param name   The requested queue name.
 * @param dup    The duplicate flag.
 *
 * @return true if the requested name already exists
 */
RexxReturnCode LocalQueueManager::openNamedQueue(const char *name, size_t *dup)
{
    bool isSession = false;

    if (!validateQueueName(name, isSession))            // make sure this is a valid name
    {
        // this might have been rejected because the name was "SESSION". The session
        // queue always exists
        if (isSession)
        {
            // this is considered a duplicate
            *dup = true;
            return RXQUEUE_OK;
        }
        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, OPEN_NAMED_QUEUE, name);

    message.send();
    // return the dup name indicator
    *dup = message.result == QUEUE_EXISTS;
    // everything worked here.
    return RXQUEUE_OK;
}

/**
 * Delete the current session queue.
 */
RexxReturnCode LocalQueueManager::deleteSessionQueue()
{
    ClientMessage message(QueueManager, DELETE_SESSION_QUEUE, sessionQueue);
    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}

/**
 * Delete a named queue.
 *
 * @param name   The name of the queue.
 */
RexxReturnCode LocalQueueManager::deleteNamedQueue(const char *name)
{
    bool isSession = false;
    if (!validateQueueName(name, isSession))            // make sure this is a valid name
    {
        // "SESSION" is considered a bad queue name in this context.
        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, DELETE_NAMED_QUEUE, name);
    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}

/**
 * Delete a named queue.
 *
 * @param name   The name of the queue.
 */
RexxReturnCode LocalQueueManager::queryNamedQueue(const char *name)
{
    bool isSession = false;
    if (!validateQueueName(name, isSession))   // make sure this is a valid name
    {
        // if the name was rejected, but is "SESSION", then we say this
        // exists.
        if (isSession)
        {
            return RXQUEUE_OK;
        }

        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, QUERY_NAMED_QUEUE, name);
    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}

/**
 * Get the count of lines in the session queue.
 *
 * @return The queue line count.
 */
RexxReturnCode LocalQueueManager::getSessionQueueCount(size_t &result)
{
    ClientMessage message(QueueManager, GET_SESSION_QUEUE_COUNT, sessionQueue);

    message.send();
    // the handle is returned in the first parameter
    result = (size_t)message.parameter1;
    // map the server result to an API return code.
    return mapReturnResult(message);
}

/**
 * Get a queue count from a named queue.
 *
 * @param name   The queue name.
 *
 * @return The count of items in the queue.
 */
RexxReturnCode LocalQueueManager::getQueueCount(const char *name, size_t &result)
{
    bool isSession = false;
    if (!validateQueueName(name, isSession))            // make sure this is a valid name
    {
        // this should never be called using the name "SESSION", so no special checks here.
        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, GET_NAMED_QUEUE_COUNT, name);
    message.send();
    // the handle is returned in the first parameter
    result = (size_t)message.parameter1;
    // map the server result to an API return code.
    return mapReturnResult(message);
}

/**
 * Remove all items from the session queue.
 */
RexxReturnCode LocalQueueManager::clearSessionQueue()
{
    ClientMessage message(QueueManager, CLEAR_SESSION_QUEUE, sessionQueue);

    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}

/**
 * Remove all items from a named queue.
 *
 * @param name   The queue name.
 */
RexxReturnCode LocalQueueManager::clearNamedQueue(const char *name)
{
    bool isSession = false;
    if (!validateQueueName(name, isSession))     // make sure this is a valid name
    {
        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, CLEAR_NAMED_QUEUE, name);

    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}


/**
 * Add an item to a named queue.
 *
 * @param name     The name of the queue.
 * @param data     The data to add
 * @param lifoFifo The lifo/fifo order flag.
 */
RexxReturnCode LocalQueueManager::addToNamedQueue(const char *name, CONSTRXSTRING &data, size_t lifoFifo)
{
    bool isSession = false;
    if (!validateQueueName(name, isSession))            // make sure this is a valid name
    {
        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, ADD_TO_NAMED_QUEUE, name);
                                           // set the additional arguments
    message.parameter1 = data.strlength;
    message.parameter2 = lifoFifo;     // make sure we have the add order

    // attach the queue item to the message.
    message.setMessageData((void *)data.strptr, data.strlength);
    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}


/**
 * Add an item to the session queue.
 *
 * @param data     The data to add.
 * @param lifoFifo The lifo/fifo flag.
 */
RexxReturnCode LocalQueueManager::addToSessionQueue(CONSTRXSTRING &data, size_t lifoFifo)
{
    ClientMessage message(QueueManager, ADD_TO_SESSION_QUEUE);

                                       // set the additional arguments
    message.parameter1 = data.strlength;
    message.parameter2 = lifoFifo;     // make sure we have the add order
    message.parameter3 = sessionQueue; // set the session handle next

    // attach the queue item to the message.
    message.setMessageData((void *)data.strptr, data.strlength);
    message.send();
    // map the server result to an API return code.
    return mapReturnResult(message);
}


RexxReturnCode LocalQueueManager::pullFromQueue(const char *name, RXSTRING &data, size_t waitFlag, RexxQueueTime *timeStamp)
{
    bool isSession = false;
    if (!validateQueueName(name, isSession))            // make sure this is a valid name
    {
        return RXQUEUE_BADQNAME;
    }

    ClientMessage message(QueueManager, PULL_FROM_NAMED_QUEUE);
    // set up for either name or session queue read
    if (name != NULL)
    {
        strcpy(message.nameArg, name);
    }
    else
    {
        message.operation = PULL_FROM_SESSION_QUEUE;
        message.parameter3 = sessionQueue;
    }
    message.parameter1 = waitFlag != 0 ? QUEUE_WAIT_FOR_DATA : QUEUE_NO_WAIT;
    message.send();
    if (message.result == QUEUE_ITEM_PULLED)
    {
        message.transferMessageData(data);
        // if this was a null string, then an empty buffer is sent back.  Allocate a minimal
        // buffer to distinguish between nothing and a null string value
        if (data.strptr == NULL)
        {
            data.strptr = (char *)RexxAllocateMemory(1);
            if (data.strptr == NULL)
            {
                throw new ServiceException(MEMORY_ERROR, "LocalQueueManager::pullFromQueue() Failure allocating memory");
            }
        }
        // if the timestamp was requested, return it.
        if (timeStamp != NULL)
        {
            memcpy(timeStamp, message.nameArg, sizeof(RexxQueueTime));
        }
    }
    // map the server result to an API return code.
    return mapReturnResult(message);
}


/**
 * Bump the usage count of a session queue when it is
 * inherited from a parent process.
 *
 * @param q      The handle of the session queue.
 */
QueueHandle LocalQueueManager::nestSessionQueue(SessionID session, QueueHandle q)
{
    ClientMessage message(QueueManager, NEST_SESSION_QUEUE, session);
    message.parameter2 = q;
    message.send();
    // we either got back the queue handle for the nested one, or a new
    // queue.
    return (QueueHandle)message.parameter1;
}


/**
 * Process an exception returned from the server and
 * map it into an API return code.
 *
 * @param e      The exception from the server.
 *
 * @return The mapped return code.
 */
RexxReturnCode LocalQueueManager::processServiceException(ServiceException *e)
{
    switch (e->getErrorCode())
    {
        case CONNECTION_FAILURE:
            return RXAPI_NORXAPI;

        case INVALID_QUEUE_NAME:
            return RXQUEUE_BADQNAME;

        case BAD_FIFO_LIFO:
            return RXQUEUE_PRIORITY;

        case BAD_WAIT_FLAG:
            return RXQUEUE_BADWAITFLAG;

        default:
            return RXQUEUE_MEMFAIL;
    }
}


/**
 * Process an exception returned from the server and
 * map it into an API return code.
 *
 * @param e      The exception from the server.
 *
 * @return The mapped return code.
 */
RexxReturnCode LocalQueueManager::mapReturnResult(ServiceMessage &m)
{
    switch (m.result)
    {
        case QUEUE_DOES_NOT_EXIST:
            return RXQUEUE_NOTREG;

        case QUEUE_IN_USE:
            return RXQUEUE_ACCESS;

        case QUEUE_EMPTY:
            return RXQUEUE_EMPTY;

        default:
            return RXQUEUE_OK;
    }
}
