/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 2008-2025 Rexx Language Association. All rights reserved.    */
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
/* THIS SOFTWARE IS PROVIDED BY THE COPYright HOLDERS AND CONTRIBUTORS        */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYright   */
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include <rexx.h>
#include <oorexxapi.h>
#include <string.h>
#include <stdio.h>

// test function for testing the function registration functions
size_t REXXENTRY TestFunction(
   const char *Name,
   size_t Argc,           /* number of arguments */
   CONSTRXSTRING Argv[],  /* list of argument strings */
   const char *Queuename, /* current queue name */
   PRXSTRING Retstr)      /* returned  */
{
    // if registered as an error tester, raise an error
    if (strcmp(Name, "TESTERROR") == 0) {
        return 40;
    }

    // return the name, count of arguments, and first argument as a return value
    snprintf(Retstr->strptr, 256, "%s %zd %s", Name, Argc, Argv[0].strptr);
    Retstr->strlength = strlen(Retstr->strptr);
    return 0;
}



RexxMethod2(RexxObjectPtr,              // Return type
            TestCreateQueue,            // Method name
            OPTIONAL_CSTRING, qname,    // Queue name
            OPTIONAL_size_t, qnameLen)  // Queue name length
{
    char newQueueName[MAX_QUEUE_NAME_LENGTH * 2]; // allow tests for names that are too long
    size_t flag;

    RexxReturnCode rc = RexxCreateQueue(newQueueName, argumentExists(2) ? qnameLen : sizeof(newQueueName),
                                        qname, &flag);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    context->SetObjectVariable("FLAG", context->StringSizeToObject(flag));
    return context->NewStringFromAsciiz(newQueueName);
}

RexxMethod1(int,                        // Return type
            TestOpenQueue,              // Method name
            CSTRING, qname)             // Queue name
{
    size_t flag;

    RexxReturnCode rc = RexxOpenQueue(qname, &flag);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    context->SetObjectVariable("FLAG", context->StringSizeToObject(flag));
    return rc;
}

RexxMethod1(int,                        // Return type
            TestQueueExists,            // Method name
            CSTRING, qname)             // Queue name
{
    RexxReturnCode rc = RexxQueueExists(qname);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    return rc;
}

RexxMethod1(int,                        // Return type
            TestDeleteQueue,            // Method name
            CSTRING, qname)             // Queue name
{
    RexxReturnCode rc = RexxDeleteQueue(qname);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    return rc;
}

RexxMethod1(int,                        // Return type
            TestQueryQueue,             // Method name
            CSTRING, qname)             // Queue name
{
    size_t count;

    RexxReturnCode rc = RexxQueryQueue(qname, &count);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    context->SetObjectVariable("FLAG", context->StringSizeToObject(count));
    return rc;
}

RexxMethod3(int,                        // Return type
            TestAddQueue,               // Method name
            CSTRING, qname,             // Queue name
            RexxStringObject, data,     // Queue data to add
            int, type)                  // Queue FIFO/LIFO flag
{
    CONSTRXSTRING rxdata;

    MAKERXSTRING(rxdata, context->StringData(data), context->StringLength(data));
    RexxReturnCode rc = RexxAddQueue(qname, &rxdata, type);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    return rc;
}

RexxMethod1(int,                        // Return type
            TestPullFromQueue,          // Method name
            CSTRING, qname)             // Queue name
{
    RXSTRING data;
    RexxQueueTime timestamp;

    MAKERXSTRING(data, NULL, 0);

    RexxReturnCode rc = RexxPullFromQueue(qname, &data, &timestamp, 0);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    if (rc == RXQUEUE_OK)
    {
        context->SetObjectVariable("FLAG", context->NewString(data.strptr, data.strlength));

        char stamp[1024];
        // the time stamp comes from a struct tm
        // year is years since 1900
        // month is 0 .. 11
        // hundredths and microseconds are not available
        snprintf(stamp, sizeof(stamp), "%d-%d-%d %d:%d:%d (%d, %d)",
          1900 + timestamp.year, 1 + timestamp.month, timestamp.day,
          timestamp.hours, timestamp.minutes, timestamp.seconds,
          timestamp.yearday, timestamp.weekday);
        context->SetObjectVariable("TIMESTAMP", context->NewString(stamp, strlen(stamp)));
    }
    return rc;
}

RexxMethod1(int,                        // Return type
            TestClearQueue,             // Method name
            CSTRING, qname)             // Queue name
{
    RexxReturnCode rc = RexxClearQueue(qname);
    context->SetObjectVariable("RETC", context->Int32ToObject(rc));
    return rc;
}

RexxMethod1(int,                        // Return type
            TestAllocateFreeMemory,     // Method name
            int, size)                  // Size of memory block
{
    void *block = RexxAllocateMemory(size);
    if (block == NULL) {
        return 1;
    }
    return RexxFreeMemory(block);
}

RexxMethod0(int,                        // Return type
            TestMVariablePool)          // Method name
{
    RexxReturnCode retc = RexxVariablePool(NULL);
    return retc;
}

RexxRoutine1(int,                       // Return type
            TestFVariablePool,          // Function name
            RexxArrayObject, arr)       // Array of shvblocks
{
    RexxReturnCode retc;
    size_t members = context->ArrayItems(arr);
    size_t ctr;
    PSHVBLOCK blocks = NULL;
    PSHVBLOCK nextblock = NULL;
    PSHVBLOCK currentblock = NULL;
    unsigned int tempint;
    RexxStringObject str;
    size_t len;

    // for a RXSHV_FETCH, RXSHV_SYFET or RXSHV_PRIV operation we may either let
    // RexxVariablePool allocate memory for a returned value, or malloc it here
    // for RexxVariablePool to use it
    // we need to remember who did which memory allocation as the one will have
    // to be freed with RexxFreeMemory and the other with free
    char* rexxAlloc = (char*)malloc((members + 1) * sizeof(char));

    // set up the shvblocks from the array
    for (ctr = 1; ctr <= members; ctr++) {
        nextblock = (PSHVBLOCK)malloc(sizeof(SHVBLOCK));
        if (currentblock != NULL) {
            currentblock->shvnext = nextblock;
        }
        currentblock = nextblock;
        if (blocks == NULL) {
            blocks = currentblock;
        }
        currentblock->shvnext = NULL;
        RexxObjectPtr entry = context->ArrayAt(arr, ctr);
        RexxObjectPtr val = context->SendMessage0(entry, "shvcode");
        context->ObjectToUnsignedInt32(val, &tempint);
        currentblock->shvcode = (unsigned char)tempint;
        val = context->SendMessage0(entry, "shvret");
        context->ObjectToUnsignedInt32(val, &tempint);
        currentblock->shvret = (unsigned char)tempint;
        val = context->SendMessage0(entry, "shvnamelen");
        context->ObjectToUnsignedInt32(val, &tempint);
        currentblock->shvnamelen = (size_t)tempint;
        val = context->SendMessage0(entry, "shvvaluelen");
        context->ObjectToUnsignedInt32(val, &tempint);
        currentblock->shvvaluelen = (size_t)tempint;

        // all request codes require a name
        val = context->SendMessage0(entry, "shvname");
        str = context->ObjectToString(val);
        currentblock->shvname.strptr = context->StringData(str);
        currentblock->shvname.strlength = context->StringLength(str);

        switch (currentblock->shvcode)
        {
        case RXSHV_SET:
        case RXSHV_SYSET:
            // a Set request also requires a value
            val = context->SendMessage0(entry, "shvvalue");
            str = context->ObjectToString(val);
            currentblock->shvvalue.strptr = (char *)context->StringData(str);
            currentblock->shvvalue.strlength = context->StringLength(str);
            break;

        case RXSHV_FETCH:
        case RXSHV_SYFET:
        case RXSHV_PRIV:
            len = currentblock->shvvaluelen;
            currentblock->shvvalue.strlength = len;
            // allocate a value buffer or let RexxVariablePool allocate it
            currentblock->shvvalue.strptr = len == 0 ? NULL: (char *)malloc(len);
            rexxAlloc[ctr] = len == 0; // remember wo did it
            break;

        case RXSHV_DROPV:
        case RXSHV_SYDRO:
        default:
            // allow invalid request codes to be handled by RexxVariablePool
            break;
        }
    }

    // call the variable pool interface
    retc = RexxVariablePool(blocks);

    // set the array to the shvblocks
    currentblock = blocks;
    for (ctr = 1; ctr <= members; ctr++)
    {
        RexxObjectPtr entry = context->ArrayAt(arr, ctr);
        context->SendMessage1(entry, "shvret=", context->UnsignedInt32ToObject((uint32_t)currentblock->shvret));
        switch (currentblock->shvcode)
        {
        case RXSHV_FETCH:
        case RXSHV_SYFET:
        case RXSHV_PRIV:
            context->SendMessage1(entry, "shvvalue=", context->NewString(currentblock->shvvalue.strptr, currentblock->shvvalue.strlength));
            context->SendMessage1(entry, "shvvaluelen=", context->UnsignedInt32ToObject((uint32_t)currentblock->shvvaluelen));
            // memory allocated by RexxVariablePool must be freed with RexxFreeMemory
            if (currentblock->shvvalue.strptr != NULL)
            {
                if (rexxAlloc[ctr] == 1)
                    RexxFreeMemory(currentblock->shvvalue.strptr);
                else
                    free(currentblock->shvvalue.strptr);
            }
            break;

        default:
            break;
        }
        nextblock = currentblock->shvnext;
        free(currentblock);
        currentblock = nextblock;
    }
    free(rexxAlloc);

    return retc;
}

RexxRoutine1(size_t,                    // Return type
            TestFNVariablePool,         // Function name
            RexxArrayObject, arr)       // Array of shvblocks

{
    RexxReturnCode retc = 0;
    size_t members = context->ArrayItems(arr);
    size_t ctr = 0;
    SHVBLOCK block;

    while (retc != RXSHV_LVAR && ctr < members) {
        block.shvnext = NULL;
        block.shvname.strptr = NULL;
        block.shvname.strlength = 0;
        block.shvvalue.strptr = NULL;
        block.shvvalue.strlength = 0;
        block.shvnamelen = 0;
        block.shvvaluelen = 0;
        block.shvcode = RXSHV_NEXTV;
        block.shvret = 0;
        retc = RexxVariablePool(&block);

        ctr++;
        RexxObjectPtr entry = context->ArrayAt(arr, ctr);
        context->SendMessage1(entry, "shvret=", context->UnsignedInt32ToObject((uint32_t)block.shvret));
        context->SendMessage1(entry, "shvname=", context->NewString(block.shvname.strptr, block.shvname.strlength));
        context->SendMessage1(entry, "shvvalue=", context->NewString(block.shvvalue.strptr, block.shvvalue.strlength));

        if (block.shvname.strptr != NULL) {
            // this memory must be freed this way since it was allocated with RexxAllocateMemeory
            RexxFreeMemory((void *)block.shvname.strptr);
        }
        if (block.shvvalue.strptr != NULL) {
            // this memory must be freed this way since it was allocated with RexxAllocateMemeory
            RexxFreeMemory(block.shvvalue.strptr);
        }
    }

    return ctr;
}

RexxMethod3(int,                        // Return type
            TestAddMacro,               // Method name
            CSTRING, name,              // Macro name
            CSTRING, filename,          // Macro file name
            size_t, srchpos)            // Search position
{
    RexxReturnCode retc = RexxAddMacro(name, filename, srchpos);
    return retc;
}

RexxMethod1(int,                        // Return type
            TestDropMacro,              // Method name
            CSTRING, name)              // Macro name
{
    RexxReturnCode retc = RexxDropMacro(name);
    return retc;
}

RexxMethod2(int,                        // Return type
            TestSaveMacroSpace,         // Method name
            RexxArrayObject, names,     // Array of macro names
            CSTRING, filename)          // Macro file name
{
    size_t argc = context->ArrayItems(names);
    const char **cnames = (const char **)calloc(argc, sizeof(const char *));

    for (size_t i = 1; i <= argc; i++) {
        cnames[i - 1] = context->ObjectToStringValue(context->ArrayAt(names, i));
    }

    RexxReturnCode retc = RexxSaveMacroSpace(argc, cnames, filename);
    free(cnames);
    return retc;
}

RexxMethod2(int,                        // Return type
            TestLoadMacroSpace,         // Method name
            RexxArrayObject, names,     // Array of macro names
            CSTRING, filename)          // Macro file name
{
    size_t argc = context->ArrayItems(names);
    const char **cnames = (const char **)calloc(argc, sizeof(const char *));

    for (size_t i = 1; i <= argc; i++) {
        cnames[i - 1] = context->ObjectToStringValue(context->ArrayAt(names, i));
    }

    RexxReturnCode retc = RexxLoadMacroSpace(argc, cnames, filename);
    free(cnames);
    return retc;
}

RexxMethod1(int,                        // Return type
            TestQueryMacro,             // Method name
            CSTRING, name)              // Macro name
{
    unsigned short pos;

    RexxReturnCode retc = RexxQueryMacro(name, &pos);
    return retc;
}

RexxMethod2(int,                        // Return type
            TestReorderMacro,           // Method name
            CSTRING, name,              // Macro name
            size_t, pos)                // New position
{
    RexxReturnCode retc = RexxReorderMacro(name, pos);
    return retc;
}

RexxMethod0(int,                        // Return type
            TestClearMacroSpace)        // Method name
{
    RexxReturnCode retc = RexxClearMacroSpace();
    return retc;
}

RexxMethod1(int,                        // Return type
            TestRegisterFunctionExe,    // Method name
            CSTRING, name)              // function name
{
    RexxReturnCode retc = RexxRegisterFunctionExe(name, (REXXPFN)TestFunction);
    return retc;
}

RexxMethod1(int,                        // Return type
            TestDeregisterFunction,    // Method name
            CSTRING, name)              // function name
{
    RexxReturnCode retc = RexxDeregisterFunction(name);
    return retc;
}

RexxMethod1(int,                        // Return type
            TestQueryFunction,    // Method name
            CSTRING, name)              // function name
{
    RexxReturnCode retc = RexxQueryFunction(name);
    return retc;
}


RexxMethodEntry orxtest_methods[] = {
    REXX_METHOD(TestCreateQueue,        TestCreateQueue),
    REXX_METHOD(TestOpenQueue,          TestOpenQueue),
    REXX_METHOD(TestQueueExists,        TestQueueExists),
    REXX_METHOD(TestDeleteQueue,        TestDeleteQueue),
    REXX_METHOD(TestQueryQueue,         TestQueryQueue),
    REXX_METHOD(TestAddQueue,           TestAddQueue),
    REXX_METHOD(TestPullFromQueue,      TestPullFromQueue),
    REXX_METHOD(TestClearQueue,         TestClearQueue),
    REXX_METHOD(TestAllocateFreeMemory, TestAllocateFreeMemory),
    REXX_METHOD(TestMVariablePool,      TestMVariablePool),
    REXX_METHOD(TestAddMacro,           TestAddMacro),
    REXX_METHOD(TestDropMacro,          TestDropMacro),
    REXX_METHOD(TestSaveMacroSpace,     TestSaveMacroSpace),
    REXX_METHOD(TestLoadMacroSpace,     TestLoadMacroSpace),
    REXX_METHOD(TestQueryMacro,         TestQueryMacro),
    REXX_METHOD(TestReorderMacro,       TestReorderMacro),
    REXX_METHOD(TestClearMacroSpace,    TestClearMacroSpace),
    REXX_METHOD(TestRegisterFunctionExe,TestRegisterFunctionExe),
    REXX_METHOD(TestDeregisterFunction, TestDeregisterFunction),
    REXX_METHOD(TestQueryFunction, TestQueryFunction),
    REXX_LAST_METHOD()
};


RexxRoutineEntry orxtest_routines[] = {
    REXX_TYPED_ROUTINE(TestFVariablePool,  TestFVariablePool),
    REXX_TYPED_ROUTINE(TestFNVariablePool, TestFNVariablePool),
    REXX_LAST_ROUTINE()
};


RexxPackageEntry UnitTest_package_entry = {
    STANDARD_PACKAGE_HEADER
    REXX_INTERPRETER_4_0_0,              // anything after 4.0.0 will work
    "UnitTest",                          // name of the package
    "1.0.0",                             // package information
    NULL,                                // no load/unload functions
    NULL,
    orxtest_routines,                    // the exported routines
    orxtest_methods                      // the exported methods
};

// package loading stub.
OOREXX_GET_PACKAGE(UnitTest);

