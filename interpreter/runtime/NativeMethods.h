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
/* REXX Kernel                                                                */
/*                                                                            */
/* list of REXX internal native methods                                       */
/*                                                                            */
/******************************************************************************/

//NOTE:  This file gets included multiple times to define different tables, so
// it does not have standard #ifndef multiple include protections.
INTERNAL_METHOD(alarm_startTimer)
INTERNAL_METHOD(alarm_stopTimer)
INTERNAL_METHOD(ticker_createTimer)
INTERNAL_METHOD(ticker_waitTimer)
INTERNAL_METHOD(ticker_stopTimer)
INTERNAL_METHOD(stream_init)
INTERNAL_METHOD(stream_chars)
INTERNAL_METHOD(stream_lines)
INTERNAL_METHOD(stream_position)
INTERNAL_METHOD(stream_state)
INTERNAL_METHOD(stream_description)
INTERNAL_METHOD(stream_query_position)
INTERNAL_METHOD(stream_charout)
INTERNAL_METHOD(stream_charin)
INTERNAL_METHOD(stream_linein)
INTERNAL_METHOD(stream_lineout)
INTERNAL_METHOD(stream_arrayin)
INTERNAL_METHOD(qualify)
INTERNAL_METHOD(query_exists)
INTERNAL_METHOD(query_size)
INTERNAL_METHOD(query_time)
INTERNAL_METHOD(handle_set)
INTERNAL_METHOD(std_set)
INTERNAL_METHOD(stream_flush)
INTERNAL_METHOD(query_handle)
INTERNAL_METHOD(query_streamtype)
INTERNAL_METHOD(stream_close)
INTERNAL_METHOD(stream_uninit)
INTERNAL_METHOD(stream_open)
INTERNAL_METHOD(rexx_create_queue)
INTERNAL_METHOD(rexx_open_queue)
INTERNAL_METHOD(rexx_queue_exists)
INTERNAL_METHOD(rexx_delete_queue)
INTERNAL_METHOD(rexx_query_queue)
INTERNAL_METHOD(rexx_push_queue)
INTERNAL_METHOD(rexx_queue_queue)
INTERNAL_METHOD(rexx_pull_queue)
INTERNAL_METHOD(rexx_linein_queue)
INTERNAL_METHOD(rexx_clear_queue)
INTERNAL_METHOD(file_separator)
INTERNAL_METHOD(file_path_separator)
INTERNAL_METHOD(file_case_sensitive)
INTERNAL_METHOD(file_list_roots)
INTERNAL_METHOD(file_qualify)
INTERNAL_METHOD(file_exists)
INTERNAL_METHOD(file_delete_file)
INTERNAL_METHOD(file_delete_directory)
INTERNAL_METHOD(file_isDirectory)
INTERNAL_METHOD(file_isFile)
INTERNAL_METHOD(file_isHidden)
INTERNAL_METHOD(file_get_last_modified)
INTERNAL_METHOD(file_set_last_modified)
INTERNAL_METHOD(file_set_read_only)
INTERNAL_METHOD(file_length)
INTERNAL_METHOD(file_list)
INTERNAL_METHOD(file_make_dir)
INTERNAL_METHOD(file_can_read)
INTERNAL_METHOD(file_can_write)
INTERNAL_METHOD(file_rename)
INTERNAL_METHOD(this_file_case_sensitive)
INTERNAL_METHOD(file_get_last_accessed)
INTERNAL_METHOD(file_set_last_accessed)
INTERNAL_METHOD(file_set_writable)
INTERNAL_METHOD(file_temporary_path)
INTERNAL_METHOD(file_search_path_impl)

