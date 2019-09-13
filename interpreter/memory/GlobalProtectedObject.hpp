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
#ifndef GlobalProtectedObject_Included
#define GlobalProtectedObject_Included

#include "RexxMemory.hpp"
#include "ActivityManager.hpp"

class RexxInstruction;

class GlobalProtectedObject
{
     friend class MemoryObject;
 public:
     inline GlobalProtectedObject() : protectedObject(OREF_NULL), next(NULL), previous(NULL)
     {
         // because this is global to all threads, we might not be at the
         // head of the list when the destructor is run, so we need to
         // maintain a double linked list.
         next = memoryObject.protectedObjects;
         if (next != NULL)
         {
             next->previous = this;
         }
         memoryObject.protectedObjects = this;
     }

     inline GlobalProtectedObject(RexxObject *o) : protectedObject(o), next(NULL), previous(NULL)
     {
         // because this is global to all threads, we might not be at the
         // head of the list when the destructor is run, so we need to
         // maintain a double linked list.
         next = memoryObject.protectedObjects;
         if (next != NULL)
         {
             next->previous = this;
         }
         memoryObject.protectedObjects = this;
     }

     inline GlobalProtectedObject(RexxInternalObject *o) : protectedObject((RexxObject *)o), next(NULL), previous(NULL)
     {
         // because this is global to all threads, we might not be at the
         // head of the list when the destructor is run, so we need to
         // maintain a double linked list.
         next = memoryObject.protectedObjects;
         if (next != NULL)
         {
             next->previous = this;
         }
         memoryObject.protectedObjects = this;
     }

     inline ~GlobalProtectedObject()
     {
         // if at the head of the chain, we just update the master pointer
         if (previous == NULL)
         {
             memoryObject.protectedObjects = next;
             // the next element has no predecessor
             if (next != NULL)
             {
                 next->previous = NULL;
             }
         }
         else
         {
             // dechain
             previous->next = next;
             if (next != NULL)
             {
                 next->previous = previous;
             }
         }
         if (protectedObject != OREF_NULL)
         {
             memoryObject.holdObject(protectedObject);
         }
     }

     inline GlobalProtectedObject & operator=(RexxInternalObject *o)
     {
         protectedObject = o;
         return *this;
     }

     inline bool operator==(RexxInternalObject *o)
     {
         return protectedObject == o;
     }

     inline bool operator!=(RexxInternalObject *o)
     {
         return protectedObject != o;
     }

     // cast conversion operators for some very common uses of protected object.
     inline operator RexxObject *()
     {
         return (RexxObject *)protectedObject;
     }

     inline operator RexxInternalObject *()
     {
         return protectedObject;
     }

     inline operator RexxObjectPtr ()
     {
         return (RexxObjectPtr)protectedObject;
     }

protected:

    RexxInternalObject *protectedObject;  // next in the chain of protected object
    GlobalProtectedObject *next;          // the pointer protected by the object
    GlobalProtectedObject *previous;      // the previous object in the chain
};
#endif
