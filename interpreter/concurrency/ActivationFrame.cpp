/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2025 Rexx Language Association. All rights reserved.    */
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

#include "RexxCore.h"
#include "ActivationFrame.hpp"
#include "RexxActivation.hpp"
#include "NativeActivation.hpp"
#include "StackFrameClass.hpp"

RexxString *RexxActivationFrame::messageName()
{
    return activation->getMessageName();
}

BaseExecutable *RexxActivationFrame::executable()
{
    return activation->getExecutableObject();
}

StackFrameClass *RexxActivationFrame::createStackFrame()
{
    return activation->createStackFrame();
}

PackageClass *RexxActivationFrame::getPackage()
{
    return activation->getEffectivePackageObject();
}

RexxObject *RexxActivationFrame::getContextObject()
{
    return activation->getContextObject();
}

RexxString *NativeActivationFrame::messageName()
{
    return activation->getMessageName();
}

BaseExecutable *NativeActivationFrame::executable()
{
    return activation->getExecutableObject();
}

StackFrameClass *NativeActivationFrame::createStackFrame()
{
    return activation->createStackFrame();
}

PackageClass *NativeActivationFrame::getPackage()
{
    return activation->getPackageObject();
}

RexxString *InternalActivationFrame::messageName()
{
    return name;
}

BaseExecutable *InternalActivationFrame::executable()
{
    return frameMethod;
}

StackFrameClass *InternalActivationFrame::createStackFrame()
{
    ArrayClass *info = new_array(name, frameMethod->getScopeName());
    ProtectedObject p(info);

    RexxString *message = activity->buildMessage(Message_Translations_compiled_method_invocation, info);
    p = message;
    Protected<ArrayClass> args = new_array(count, argPtr);
    return new StackFrameClass(StackFrameClass::FRAME_METHOD, name, frameMethod, target, args, message, SIZE_MAX, 0, OREF_NULL);
}

PackageClass *InternalActivationFrame::getPackage()
{
    return OREF_NULL;
}

RexxString *CompileActivationFrame::messageName()
{
    return OREF_NULL;
}

BaseExecutable *CompileActivationFrame::executable()
{
    return OREF_NULL;
}

StackFrameClass *CompileActivationFrame::createStackFrame()
{
    return parser->createStackFrame();
}

PackageClass *CompileActivationFrame::getPackage()
{
    return parser->getPackage();
}
