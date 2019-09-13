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
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "RexxCore.h"
#include "StringClass.hpp"
#include "SystemInterpreter.hpp"

#define ACCOUNT_BUFFER_SIZE  256

/*********************************************************************/
/*                                                                   */
/*   Subroutine Name:   SysUserid                                    */
/*                                                                   */
/*   Descriptive Name:  retrieve current userid                      */
/*                                                                   */
/*********************************************************************/

RexxString *SystemInterpreter::getUserid()
{
    char account_buffer[ACCOUNT_BUFFER_SIZE];
#if defined( HAVE_GETPWUID )
    struct passwd * pstUsrDat;
#endif

    account_buffer[ACCOUNT_BUFFER_SIZE-1] = '\0'; /* Max delimiter            */
#if defined( HAVE_GETPWUID )
    pstUsrDat = getpwuid(geteuid());
    strncpy( account_buffer,  pstUsrDat->pw_name, ACCOUNT_BUFFER_SIZE-1);
#elif defined( HAVE_IDTOUSER )
    strncpy( account_buffer, IDtouser(geteuid()), ACCOUNT_BUFFER_SIZE-1);
#else
    strcpy( account_buffer, "unknown" );
#endif
    return new_string(account_buffer);
}
