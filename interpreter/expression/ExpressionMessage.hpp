/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
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
/******************************************************************************/
/* REXX Kernel                                       ExpressionMessage.hpp    */
/*                                                                            */
/* Primitive Expression Message Send Class Definitions                        */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxExpressionMessage
#define Included_RexxExpressionMessage

class LanguageParser;

class RexxExpressionMessage : public RexxVariableBase
{
 friend class RexxInstructionMessage;
 friend class LanguageParser;
 public:
    void *operator new(size_t, size_t);
    inline void  operator delete(void *) { ; }

    RexxExpressionMessage(RexxInternalObject *, RexxString *, RexxInternalObject *, size_t, QueueClass *, bool);
    inline RexxExpressionMessage(RESTORETYPE restoreType) { ; };

    void live(size_t) override;
    void liveGeneral(MarkReason reason) override;
    void flatten(Envelope *) override;

    RexxObject *evaluate(RexxActivation *, ExpressionStack *) override;
    void assign(RexxActivation *, RexxObject *) override;

    void makeAssignment(LanguageParser *parser);
    bool isDoubleTilde() { return doubleTilde; }

 protected:

    RexxInternalObject * target;         // target subexpression
    RexxString *messageName;             // the message name
    RexxInternalObject *super;           // super class target
    bool   doubleTilde;                  // this is the double tilde form
    size_t argumentCount;                // number of message arguments
    RexxInternalObject *arguments[1];    // list of argument subexpressions
};
#endif
