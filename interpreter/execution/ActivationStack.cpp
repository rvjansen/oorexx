/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2019 Rexx Language Association. All rights reserved.    */
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
/******************************************************************************/
/*                                                                            */
/* Primitive Activation Frame Stack support classes                           */
/*                                                                            */
/* NOTE:  activations are an execution time only object.  They are never      */
/*        flattened or saved in the image, and hence will never be in old     */
/*        space.  Because of this, activations "cheat" and do not use         */
/*        OrefSet to assign values to get better performance.  Care must be   */
/*        used to maintain this situation.                                    */
/*                                                                            */
/******************************************************************************/

#include <algorithm>
#include "RexxCore.h"
#include "ActivationStack.hpp"



/**
 * Allocate memory for a new frame buffer.
 *
 * @param size    The base size of the object.
 * @param entries The number of entries we need to allocate space for.
 *
 * @return The backing storage for a frame buffer.
 */
void *ActivationFrameBuffer::operator new(size_t size, size_t entries)
{
    return new_object(size + (entries * sizeof(RexxObject *)), T_ActivationFrameBuffer);
}


/**
 * Constructor for a frame buffer.
 *
 * @param entries The number of entries in the frame buffer.
 */
ActivationFrameBuffer::ActivationFrameBuffer(size_t entries)
{
    size = entries;
    next = 0;
    previous = OREF_NULL;
}


/**
 * Perform garbage collection on a live object.
 *
 * @param liveMark The current live mark.
 */
void ActivationFrameBuffer::live(size_t liveMark)
{
    // we only mark housekeeping type fields.  The main buffer
    // entries are marked by the owning activations.
    memory_mark(previous);
}


/**
 * Perform generalized live marking on an object.  This is
 * used when mark-and-sweep processing is needed for purposes
 * other than garbage collection.
 *
 * @param reason The reason for the marking call.
 */
void ActivationFrameBuffer::liveGeneral(MarkReason reason)
{
    memory_mark_general(previous);
}


/**
 * Perform garbage collection on a live object.
 *
 * @param liveMark The current live mark.
 */
void ActivationStack::live(size_t liveMark)
{
    memory_mark(current);
    memory_mark(unused);
}


/**
 * Perform generalized live marking on an object.  This is
 * used when mark-and-sweep processing is needed for purposes
 * other than garbage collection.
 *
 * @param reason The reason for the marking call.
 */
void ActivationStack::liveGeneral(MarkReason reason)
{
    memory_mark_general(current);
    memory_mark_general(unused);
}


/**
 * Initialize a frame stack for a new activity.
 */
void ActivationStack::init()
{
    current = new_activationFrameBuffer(DefaultFrameBufferSize);
    unused = OREF_NULL;
}


/**
 * Expand the capacity of the stack to add at least entries
 * additional values on the stack.
 *
 * @param entries
 */
void ActivationStack::expandCapacity(size_t entries)
{
    ActivationFrameBuffer *next;
    entries = std::max(entries, (size_t)DefaultFrameBufferSize);
    // do we have an unused one we're holding ready that has enough room?
    if (unused != OREF_NULL && unused->hasCapacity(entries))
    {
        // just activate this one for use
        next = unused;
        unused = OREF_NULL;
    }
    else
    {
        // create a new frame buffer
        next = new_activationFrameBuffer(entries);
    }
    // chain the existing buffer off of the new one
    next->push(current);
    // set this up as the new current stack
    current = next;
}
