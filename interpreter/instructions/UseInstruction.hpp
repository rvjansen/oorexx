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
/* REXX Kernel                                       UseInstruction.hpp       */
/*                                                                            */
/* Primitive USE instruction Class Definitions                                */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxInstructionUse
#define Included_RexxInstructionUse

#include "RexxInstruction.hpp"

class UseVariable
{
public:
    RexxVariableBase   *variable;      // the variable accessor
    RexxInternalObject *defaultValue;  // default value for optional variables
    void handleArgument(RexxActivation *context, ExpressionStack *stack, RexxObject *argument, size_t argumentPos, bool isStrict);
    void handleReferenceArgument(RexxActivation *context, ExpressionStack *stack, RexxObject *argument, size_t argumentPos);
};


class RexxInstructionUse : public RexxInstruction
{
 public:
    RexxInstructionUse(size_t, bool, bool, QueueClass *, QueueClass *);
    inline RexxInstructionUse(RESTORETYPE restoreType) { ; };

    void live(size_t) override;
    void liveGeneral(MarkReason reason) override;
    void flatten(Envelope *) override;

    void execute(RexxActivation *, ExpressionStack *) override;

protected:

    RexxObject *getArgument(RexxObject **arglist, size_t count, size_t target);

    size_t variableCount;            // count of variables to process
    size_t minimumRequired;          // the minimum number of require arguments
    bool variableSize;               // additional arguments allowed after last
    bool strictChecking;             // determines whether to apply strict argument checks
    UseVariable variables[1];        // List of variables for USE
};
#endif

