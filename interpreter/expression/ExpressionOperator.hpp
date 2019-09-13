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
/******************************************************************************/
/* REXX Kernel                                       ExpressionOperator.hpp   */
/*                                                                            */
/* Primitive Expression Operator Class Definitions                            */
/*                                                                            */
/******************************************************************************/
#ifndef Included_RexxExpressionOperator
#define Included_RexxExpressionOperator

#include "Token.hpp"

class RexxExpressionOperator : public RexxInternalObject
{
 public:
    inline RexxExpressionOperator() { ; }

    RexxExpressionOperator(TokenSubclass, RexxInternalObject *, RexxInternalObject *);
    inline RexxExpressionOperator(RESTORETYPE restoreType) { ; };

    void   live(size_t) override;
    void   liveGeneral(MarkReason reason) override;
    void   flatten(Envelope *) override;

    inline const char *operatorName() { return operatorNames[oper]; }

protected:
    // table of operator names
    static const char *operatorNames[];

    TokenSubclass  oper;                 // operation to perform
    RexxInternalObject *right_term;      // right term of the operator
    RexxInternalObject *left_term;       // left term of the operator
};

class RexxBinaryOperator : public RexxExpressionOperator
{
 public:
    void  *operator new(size_t);
    inline void  operator delete(void *) { ; }

    inline RexxBinaryOperator(TokenSubclass op, RexxInternalObject *left, RexxInternalObject *right)
        : RexxExpressionOperator(op, left, right) { ; }
    inline RexxBinaryOperator(RESTORETYPE restoreType) { ; };

    RexxObject *evaluate(RexxActivation *, ExpressionStack *) override;
};


class RexxUnaryOperator : public RexxExpressionOperator {
 public:
    void  *operator new(size_t);
    inline void  operator delete(void *) { ; }

    inline RexxUnaryOperator(TokenSubclass op, RexxInternalObject *left)
        : RexxExpressionOperator(op, left, OREF_NULL) { ; }
    inline RexxUnaryOperator(RESTORETYPE restoreType) { ; };

    RexxObject *evaluate(RexxActivation *, ExpressionStack *) override;
};
#endif
