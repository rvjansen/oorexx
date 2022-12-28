/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2019 Rexx Language Association. All rights reserved.    */
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

#ifndef RexxInternalApis_Included
#define RexxInternalApis_Included

// These are private APIs for communication between ooRexx components.

/*********************************************************************/
/* Defines used by SysStemSort --                                    */
/*********************************************************************/
#define SORT_CASESENSITIVE 0
#define SORT_CASEIGNORE    1

#define SORT_ASCENDING 0
#define SORT_DECENDING 1


#ifdef __cplusplus
extern "C"
{
#endif

int REXXENTRY RexxResolveExit(const char *, REXXPFN *);
int REXXENTRY RexxResolveRoutine(const char *, REXXPFN *);

RexxReturnCode REXXENTRY RexxResolveMacroFunction (const char *, PRXSTRING );
void REXXENTRY RexxCreateInterpreterImage(const char *t);

RexxReturnCode REXXENTRY RexxLoadSubcom(const char *, const char *);
RexxReturnCode REXXENTRY RexxResolveSubcom(const char *name, REXXPFN *);
RexxReturnCode RexxEntry RexxCreateSessionQueue();
RexxReturnCode RexxEntry RexxDeleteSessionQueue();
RexxReturnCode REXXENTRY RexxStemSort(RexxStemObject stem, const char *tailExtension, int order, int type,
    wholenumber_t start, wholenumber_t end, wholenumber_t firstcol, wholenumber_t lastcol);
const char* REXXENTRY RexxGetErrorMessage(int number);
const char* REXXENTRY RexxGetErrorMessageByNumber(int number);
RexxReturnCode REXXENTRY RexxCompileProgram(const char *input, const char *output, PRXSYSEXIT, bool encode);

#ifdef __cplusplus
}
#endif

#endif

