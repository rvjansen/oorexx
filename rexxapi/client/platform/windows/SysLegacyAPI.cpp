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
/*********************************************************************/
#include "rexx.h"

/*********************************************************************/
/*                                                                   */
/*  Function:         RexxPullQueue()                                */
/*                                                                   */
/*  Description:      Pull an entry from a queue.                    */
/*                                                                   */
/*  Function:         Locate the queue, return its top entry to      */
/*                    the caller, and tell the queue data            */
/*                    manager to delete the entry.                   */
/*                                                                   */
/*                    If the queue is empty, the caller can elect    */
/*                    to wait for someone to post an entry.          */
/*                                                                   */
/*  Notes:            Caller is responsible for freeing the returned */
/*                    memory.                                        */
/*                                                                   */
/*                    The entry's control block is stored in the     */
/*                    entry's memory.  We must therefore obtain      */
/*                    addressability to the entry's memory before    */
/*                    we can process the entry.                      */
/*                                                                   */
/*  Input:            external queue name, wait flag.                */
/*                                                                   */
/*  Output:           queue element, data size, date/time stamp.     */
/*                                                                   */
/*  Effects:          Top entry removed from the queue.  Message     */
/*                    queued to the queue data manager.              */
/*                                                                   */
/*********************************************************************/
RexxReturnCode REXXENTRY RexxPullQueue(
    const char *name,
    PRXSTRING   data_buf,
    SYSTEMTIME *dt,
    size_t waitflag)
{
    RexxQueueTime time;

    RexxReturnCode rc = RexxPullFromQueue(name, data_buf, &time, waitflag);
    // we need to convert the time stamp, if requested
    if (dt != NULL)
    {
        dt->wYear = time.year;
        dt->wMonth = time.month;
        dt->wDay = time.day;
        dt->wHour = time.hours;
        dt->wMinute = time.minutes;
        dt->wSecond = time.seconds;
        dt->wMilliseconds = (WORD)(time.microseconds / 1000);
        dt->wDayOfWeek = time.weekday;
    }
    return rc;
}
