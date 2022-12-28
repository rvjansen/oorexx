/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2020 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* https://www.oorexx.org/license.html                                        */
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


#include "LocalRegistrationManager.hpp"
#include "LocalAPIManager.hpp"
#include "SysLibrary.hpp"
#include "ClientMessage.hpp"
#include "Utilities.hpp"
#include "RegistrationTable.hpp"



LocalRegistrationManager::LocalRegistrationManager() : locked(false), LocalAPISubsystem()
{
    // no state in this
}

/**
 * Register a DLL-based callback type.
 *
 * @param type     The type of callback being registered
 * @param name     The name of the callback (unique within the type)
 * @param module   The name of the library containing the callback.
 * @param proc     The name of the callback entry point within the library.
 * @param userData Pointer to userdata saved with the registered callback.
 * @param drop     The drop authority.
 *
 * @return The return code for the registration.
 */
RexxReturnCode LocalRegistrationManager::registerCallback(RegistrationType type, const char *name,
    const char *module, const char *proc, const char *userData, bool drop)
{
    // first parameter for these calls is ALWAYS the type, second is always the name
    ClientMessage message(RegistrationManager, REGISTER_LIBRARY, type, name);

    // we have a secondary data area to send
    ServiceRegistrationData regData(module, proc, drop, userData);
    message.setMessageData(&regData, sizeof(ServiceRegistrationData));

    message.send();
    return mapReturnResult(message);
}

/**
 * Register an in-process callback handler.
 *
 * @param type       The type of handler.
 * @param name       The name of the handler.
 * @param entryPoint The callback entry point.
 * @param userData   The optional userdata pointer.
 *
 * @return The message return code.
 */
RexxReturnCode LocalRegistrationManager::registerCallback(RegistrationType type, const char *name, REXXPFN entryPoint,
    const char *userData)
{
    // first parameter for these calls is ALWAYS the type
    ClientMessage message(RegistrationManager, REGISTER_ENTRYPOINT, type, name);

    // and fill in the secondary data area
    ServiceRegistrationData regData(entryPoint, userData);
    message.setMessageData((char *)&regData, sizeof(ServiceRegistrationData));

    // we handle the exe callback registration locally.  So, this is a simulated SEND operation
    RegistrationTable &table(locateTable(type));

    // perform the registration locally
    table.registerCallback(message);
    return mapReturnResult(message);
}

/**
 * Locate the target table for a registration based on the type.
 *
 * @param type   The target registration type
 *
 * @return A reference to the registration table.
 */
RegistrationTable &LocalRegistrationManager::locateTable(RegistrationType type)
{
    // if this is the first request for local registration information, we need to
    // lock the rxapi library into memory by requesting an additional load.  This
    // is necessary because some apps dynamically load rexxapi, call a function, then
    // unload the library.  This wipes out the local registration tables.  By doing a
    // second load, then we are protected until the process goes away.
    if (!locked)
    {
        SysLibrary lib;
        // not necessarily an error if this fails, but we will be subject to
        // the loading/unloading behavior.  It is possible this might not be
        // an issue for most applications, so an error doesn't really seem necessary.
        if (lib.load("rexxapi"))
        {
            locked = true;
        }
    }

    if (type == FunctionAPI)
    {
        return functions;
    }
    else if (type == SubcomAPI)
    {
        return commandHandlers;
    }
    else {
        return exits ;
    }
}


/**
 * Drop a registered callback.
 *
 * @param type   The type of callback to process.
 * @param name   The name of the callback.
 * @param module An optional library qualifier
 *
 * @return The operation return code.
 */
RexxReturnCode LocalRegistrationManager::dropCallback(RegistrationType type, const char *name, const char *module)
{
    // this is a different operation depending on whether we have a module specified
    if (module != NULL)
    {
        // first parameter for these calls is ALWAYS the type.  Library registrations are
        // handled by the server, so this is always a send.
        ClientMessage message(RegistrationManager, REGISTER_DROP_LIBRARY, type, name);
        // we have extra data to send
        ServiceRegistrationData regData(module);
        message.setMessageData((char *)&regData, sizeof(ServiceRegistrationData));

        message.send();
        return mapReturnResult(message);
    }
    else
    {
        // first parameter for these calls is ALWAYS the type
        ClientMessage message(RegistrationManager, REGISTER_DROP, type, name);

        RegistrationTable &table(locateTable(type));

        // try to drop this locally
        table.dropCallback(message);
        RexxReturnCode rc = mapReturnResult(message);
        // if handled ok at this level, we're done
        if (rc == RXSUBCOM_OK)
        {
            return RXSUBCOM_OK;
        }

        // need to send this to the server.
        message.send();
        return mapReturnResult(message);
    }
}


/**
 * Perform an existance query for a callback
 *
 * @param type   The type of callback to check.
 * @param name   The callback name.
 *
 * @return The query return code.
 */
RexxReturnCode LocalRegistrationManager::queryCallback(RegistrationType type, const char *name)
{
    // first parameter for these calls is ALWAYS the type
    ClientMessage message(RegistrationManager, REGISTER_QUERY, type, name);

    RegistrationTable &table(locateTable(type));

    // try to drop this locally
    table.queryCallback(message);
    RexxReturnCode rc = mapReturnResult(message);
    // if handled ok at this level, we're done
    if (rc == RXSUBCOM_OK)
    {
        return RXSUBCOM_OK;
    }

    // need to send this to the server.

    message.send();
    // the returned extra message data is released automatically.
    return mapReturnResult(message);
}

/**
 * Perform a callback query, retrieving the userdata
 * provided when the callback was registered.
 *
 * @param type     The registration type.
 * @param name     The name of the callback,
 * @param module   The optional target library.
 * @param userData The pointer for the returned userdata.
 *
 * @return The service return code.
 */
RexxReturnCode LocalRegistrationManager::queryCallback(RegistrationType type, const char *name, const char *module,
    char *userData)
{
    if (module != NULL)
    {
        // first parameter for these calls is ALWAYS the type
        ClientMessage message(RegistrationManager, REGISTER_QUERY_LIBRARY, type, name);
        ServiceRegistrationData regData(module);

        message.setMessageData((char *)&regData, sizeof(ServiceRegistrationData));

        message.send();
        // if this was there, copy the user information back
        if (message.result == CALLBACK_EXISTS)
        {
            ServiceRegistrationData *retData = (ServiceRegistrationData *)message.getMessageData();

            retData->retrieveUserData(userData);
        }
        // the returned extra message data is released automatically.
        return mapReturnResult(message);
    }
    else
    {
        // first parameter for these calls is ALWAYS the type
        ClientMessage message(RegistrationManager, REGISTER_QUERY, type, name);
        RegistrationTable &table(locateTable(type));

        // try to drop this locally
        table.queryCallback(message);
        // if this wasn't found locally, ask the server
        if (message.result != CALLBACK_EXISTS)
        {
            // need to send this to the server.
            message.send();
        }

        // if this was there, copy the user information back
        if (message.result == CALLBACK_EXISTS)
        {
            ServiceRegistrationData *retData = (ServiceRegistrationData *)message.getMessageData();

            retData->retrieveUserData(userData);
        }
        // the returned extra message data is released automatically.
        return mapReturnResult(message);
    }
}

/**
 * Resolve a registered callback entry point.
 *
 * @param type       The registration type
 * @param name       The name of the callback.
 * @param module     An optional library qualifier.
 * @param entryPoint Pointer for returning the entry point address.
 */
RexxReturnCode LocalRegistrationManager::resolveCallback(RegistrationType type, const char *name, const char *module,
    REXXPFN &entryPoint)
{
    entryPoint = NULL;                 // assume failure

    // first parameter for these calls is ALWAYS the type
    ClientMessage message(RegistrationManager, REGISTER_LOAD_LIBRARY, type, name);

    RegistrationTable &table(locateTable(type));

    // try to locate this locally
    table.queryCallback(message);
    // if this wasn't found locally, ask the server
    if (message.result != CALLBACK_EXISTS)
    {
        // need to send this to the server.
        message.send();
    }

    // if this was there, now try to load the module, if necessary.
    if (message.result == CALLBACK_EXISTS)
    {
        ServiceRegistrationData *retData = (ServiceRegistrationData *)message.getMessageData();
        if (strlen(retData->moduleName) != 0)
        {
            entryPoint = NULL;
            SysLibrary lib;
            if (lib.load(retData->moduleName))
            {
                entryPoint = (REXXPFN)lib.getProcedure(retData->procedureName);
                if (entryPoint == NULL)
                {
                    // uppercase the name in place (this is local storage, so it's safe)
                    // and try again to resolve this
                    Utilities::strupper(retData->procedureName);
                    entryPoint = (REXXPFN)lib.getProcedure(retData->procedureName);
                    if (entryPoint == NULL)
                    {
                        return RXSUBCOM_NOTREG;
                    }
                }
            }
            else
            {
                return RXSUBCOM_NOTREG;
            }
        }
        else
        {
            entryPoint = (REXXPFN)retData->entryPoint;
        }
    }
    // the returned extra message data is released automatically.
    return mapReturnResult(message);
}


/**
 * Convert an exception returned from the service into a
 * return code.
 *
 * @param e      The service exception.
 *
 * @return The mapped return code.
 */
RexxReturnCode LocalRegistrationManager::processServiceException(ServiceException *e)
{
    switch (e->getErrorCode())
    {
        case CONNECTION_FAILURE:
            return RXAPI_NORXAPI;

        default:
            return RXAPI_MEMFAIL;
    }
    return RXAPI_MEMFAIL;
}


/**
 * Process an result returned from the server and
 * map it into an API return code.
 *
 * @param m      The return message.
 *
 * @return The mapped return code.
 */
RexxReturnCode LocalRegistrationManager::mapReturnResult(ServiceMessage &m)
{
    switch (m.result)
    {
        case CALLBACK_NOT_FOUND:
            return RXSUBCOM_NOTREG;

        case DUPLICATE_REGISTRATION:
            return RXSUBCOM_DUP;

        case DROP_NOT_AUTHORIZED:
            return RXSUBCOM_NOCANDROP;

        // CALLBACK_EXISTS is a good result and falls through
        default:
            return RXSUBCOM_OK;
    }
}
