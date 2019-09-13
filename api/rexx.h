/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2018 Rexx Language Association. All rights reserved.    */
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

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Module Name: rexx.h                                                        */
/*                                                                            */
/* ooRexx Common Definitions File                                             */
/*                                                                            */
/* Note: This is a revision of the original IBM rexx.h header file. All of the*/
/* conditional sections have been removed and it has been split into multiple */
/* header files, some of which are platform specific. Many of the types have  */
/* been changed to more portable types.                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef REXXSAA_INCLUDED
#define REXXSAA_INCLUDED


#ifdef __cplusplus
#define BEGIN_EXTERN_C() extern "C" {
#define END_EXTERN_C() }
#else
#define BEGIN_EXTERN_C()
#define END_EXTERN_C()
#endif


#include "rexxapitypes.h"              // Platform specific stuff

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                               Common                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

typedef int RexxReturnCode;           // API return type

/******************************************************************************/
/* Types (for general use)                                                    */
/******************************************************************************/
typedef const char *CSTRING;          /* pointer to zero-terminated string */
typedef void *POINTER;

#ifdef __cplusplus

class _RexxObjectPtr {};
class _RexxStringObject : public _RexxObjectPtr {};
class _RexxBufferStringObject : public _RexxStringObject {};
class _RexxArrayObject : public _RexxObjectPtr {};
class _RexxBufferObject : public _RexxObjectPtr {};
class _RexxPointerObject : public _RexxObjectPtr {};
class _RexxMethodObject : public _RexxObjectPtr {};
class _RexxRoutineObject : public _RexxObjectPtr {};
class _RexxPackageObject : public _RexxObjectPtr {};
class _RexxClassObject : public _RexxObjectPtr {};
class _RexxDirectoryObject : public _RexxObjectPtr {};
class _RexxStringTableObject : public _RexxObjectPtr {};
class _RexxSupplierObject : public _RexxObjectPtr {};
class _RexxStemObject : public _RexxObjectPtr {};
class _RexxMutableBufferObject : public _RexxObjectPtr {};
class _RexxVariableReferenceObject : public _RexxObjectPtr {};

typedef _RexxObjectPtr *RexxObjectPtr;
typedef _RexxStringObject *RexxStringObject;
typedef _RexxBufferStringObject *RexxBufferStringObject;
typedef _RexxArrayObject *RexxArrayObject;
typedef _RexxBufferObject *RexxBufferObject;
typedef _RexxPointerObject *RexxPointerObject;
typedef _RexxMethodObject *RexxMethodObject;
typedef _RexxRoutineObject *RexxRoutineObject;
typedef _RexxPackageObject *RexxPackageObject;
typedef _RexxClassObject *RexxClassObject;
typedef _RexxDirectoryObject *RexxDirectoryObject;
typedef _RexxStringTableObject *RexxStringTableObject;
typedef _RexxSupplierObject *RexxSupplierObject;
typedef _RexxStemObject *RexxStemObject;
typedef _RexxMutableBufferObject *RexxMutableBufferObject;
typedef _RexxVariableReferenceObject *RexxVariableReferenceObject;
#else
struct _RexxObjectPtr;
struct _RexxStringObject;
struct _RexxArrayObject;
struct _RexxBufferObject;
struct _RexxPointerObject;
struct _RexxMethodObject;
struct _RexxRoutineObject;
struct _RexxPackageObject;
struct _RexxClassObject;
struct _RexxDirectoryObject;
struct _RexxStringTableObject;
struct _RexxSupplierObject;
struct _RexxStemObject;
struct _RexxMutableBufferObject;
struct _RexxVariableReferenceObject;

typedef struct _RexxObjectPtr *RexxObjectPtr;
typedef struct _RexxStringObject *RexxStringObject;
typedef struct _RexxBufferStringObject *RexxBufferStringObject;
typedef struct _RexxArrayObject *RexxArrayObject;
typedef struct _RexxBufferObject *RexxBufferObject;
typedef struct _RexxPointerObject *RexxPointerObject;
typedef struct _RexxMethodObject *RexxMethodObject;
typedef struct _RexxRoutineObject *RexxRoutineObject;
typedef struct _RexxPackageObject *RexxPackageObject;
typedef struct _RexxClassObject *RexxClassObject;
typedef struct _RexxDirectoryObject *RexxDirectoryObject;
typedef struct _RexxStringTableObject *RexxStringTableObject;
typedef struct _RexxSupplierObject *RexxSupplierObject;
typedef struct _RexxStemObject *RexxStemObject;
typedef struct _RexxMutableBufferObject *RexxMutableBufferObject;
typedef struct _RexxVariableReferenceObject *RexxVariableReferenceObject;
#endif

/******************************************************************************/
/* Constant values (for general use)                                          */
/******************************************************************************/
#define NO_CSTRING            NULL
#define NULLOBJECT            NULL


/*----------------------------------------------------------------------------*/
/***    RXSTRING defines                                                      */
/*----------------------------------------------------------------------------*/

/***    Structure for external interface string (RXSTRING) */

typedef struct _RXSTRING {             /* rxstr                      */
        size_t  strlength;             /*   length of string         */
        char   *strptr;                /*   pointer to string        */
} RXSTRING;

typedef struct _CONSTRXSTRING {        /* const rxstr                */
    size_t  strlength;                 /*   length of string         */
    const char   *strptr;              /*   pointer to string        */
} CONSTRXSTRING;

#ifndef OOREXX_COMPATIBILITY
#define CONSTANT_RXSTRING CONSTRXSTRING
#define CONSTANT_STRING   CSTRING
#else
typedef char *PSZ;
#define CONSTANT_RXSTRING RXSTRING
#define CONSTANT_STRING   PSZ
#endif

/***    Macros for RexxString manipulation                   */

#define RXNULLSTRING(r)      ((r).strptr == NULL)
#define RXZEROLENSTRING(r)   ((r).strptr != NULL && (r).strlength == 0)
#define RXVALIDSTRING(r)     ((r).strptr != NULL && (r).strlength != 0)
#define RXSTRLEN(r)          (RXNULLSTRING(r) ? 0 : (r).strlength)
#define RXSTRPTR(r)          ((r).strptr)
#define MAKERXSTRING(r,p,l)  { (r).strptr = p; (r).strlength = l; }


typedef RXSTRING          *PRXSTRING;      /* pointer to a RXSTRING  */
typedef CONSTANT_RXSTRING *PCONSTRXSTRING; /* pointer to a RXSTRING  */

/***    Structure for system exit block (RXSYSEXIT) */

typedef struct _RXSYSEXIT {            /* syse */
   CONSTANT_STRING sysexit_name;       /* subcom enviro for sysexit  */
   int             sysexit_code;       /* sysexit function code      */
}  RXSYSEXIT;
typedef RXSYSEXIT *PRXSYSEXIT;         /* pointer to a RXSYSEXIT     */



/*----------------------------------------------------------------------------*/
/***    Shared Variable Pool Interface defines                                */
/*----------------------------------------------------------------------------*/

/***    Structure of Shared Variable Request Block (SHVBLOCK) */

typedef struct _SHVBLOCK {            /* shvb */
    struct _SHVBLOCK  *shvnext;       /* pointer to the next block   */
    CONSTANT_RXSTRING  shvname;       /* Pointer to the name buffer  */
    RXSTRING           shvvalue;      /* Pointer to the value buffer */
    size_t             shvnamelen;    /* Length of the name value    */
    size_t             shvvaluelen;   /* Length of the fetch value   */
    unsigned char      shvcode;       /* Function code for this block*/
    unsigned char      shvret;        /* Individual Return Code Flags*/
}   SHVBLOCK;
typedef SHVBLOCK *PSHVBLOCK;

typedef char *PEXIT;                  /* ptr to exit parameter block */




/*----------------------------------------------------------------------------*/
/***    Include the other common and platform specific stuff                  */
/*----------------------------------------------------------------------------*/

/* These must be placed after RXSTRING and CONSTRXSTRING are defined */
#include "rexxapidefs.h"
#include "rexxplatformdefs.h"          // Platform specific stuff

typedef size_t  stringsize_t;          // a Rexx string size
typedef ssize_t wholenumber_t;         // a Rexx whole number
typedef size_t  logical_t;             // a Rexx logical (1 or 0) value

// a synonym for readability
#define RexxEntry REXXENTRY


/***    RexxPullFromQueue - Retrieve data from an External Data Queue */
typedef struct _RexxQueueTime  {       /* REXX time stamp format            */
  uint16_t       hours;                /* hour of the day (24-hour)         */
  uint16_t       minutes;              /* minute of the hour                */
  uint16_t       seconds;              /* second of the minute              */
  uint16_t       hundredths;           /* hundredths of a second            */
  uint16_t       day;                  /* day of the month                  */
  uint16_t       month;                /* month of the year                 */
  uint16_t       year;                 /* current year                      */
  uint16_t       weekday;              /* day of the week                   */
  uint32_t       microseconds;         /* microseconds                      */
  uint32_t       yearday;              /* day number within the year        */
} RexxQueueTime;


typedef struct _RexxConditionData
{
  wholenumber_t code;                 // The condition CODE information
  wholenumber_t rc;                   // The condition RC value
  RXSTRING message;                   // The condition secondary message text
  RXSTRING errortext;                 // The condition error text.
  size_t  position;                   // The failure line number value
  RXSTRING program;                   // The running program name
} RexxConditionData;

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                               32-bit                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/***    Main Entry Point to the Rexx Interpreter                              */
/*----------------------------------------------------------------------------*/

BEGIN_EXTERN_C()

int REXXENTRY RexxStart (
         size_t,                       /* Num of args passed to rexx */
         PCONSTRXSTRING,               /* Array of args passed to rex */
         CONSTANT_STRING,              /* [d:][path] filename[.ext]  */
         PRXSTRING,                    /* Loc of rexx proc in memory */
         CONSTANT_STRING,              /* ASCIIZ initial environment.*/
         int,                          /* type (command,subrtn,funct) */
         PRXSYSEXIT,                   /* SysExit env. names &  codes */
         short *,                      /* Ret code from if numeric   */
         PRXSTRING );                  /* Retvalue from the rexx proc */
typedef RexxReturnCode (REXXENTRY *PFNREXXSTART)(size_t, PCONSTRXSTRING, CONSTANT_STRING, PRXSTRING,
                                        CONSTANT_STRING, int, PRXSYSEXIT, short *,
                                        PRXSTRING);
#define REXXSTART RexxStart

// the following APIs are deprecated, and are included only for binary compatibility.
// These are nops if called.
void REXXENTRY RexxWaitForTermination(void);
typedef void (REXXENTRY *PFNREXXWAITFORTERMINATION)(void);
#define REXXWAITFORTERMINATION RexxWaitForTermination

RexxReturnCode REXXENTRY RexxDidRexxTerminate(void);
typedef RexxReturnCode (REXXENTRY *PFNREXXDIDREXXTERMINATE)(void);
#define REXXDIDREXXTERMINATE RexxDidRexxTerminate

RexxReturnCode REXXENTRY RexxShutDownAPI(void);
typedef RexxReturnCode (REXXENTRY *PFNREXXSHUTDOWNAPI)(void);
#define REXXSHUTDOWNAPI RexxShutDownAPI


RexxReturnCode REXXENTRY RexxTranslateProgram(
    CONSTANT_STRING,                    // input program name
    CONSTANT_STRING,                    // output file name
    PRXSYSEXIT);                        // system exits to use during translation


typedef RexxReturnCode (REXXENTRY *PFNREXXTRANSLATEPROGRAM)(CONSTANT_STRING, CONSTANT_STRING, PRXSYSEXIT);

#define REXXTRANSLATEPROGRAM RexxTranslateProgram


RexxReturnCode REXXENTRY RexxTranslateInstoreProgram(
    CONSTANT_STRING,                    // input program name
    CONSTANT_RXSTRING *,                // program source
    RXSTRING *);                        // returned image


typedef RexxReturnCode (REXXENTRY *PFNREXXTRANSLATEINSTOREPROGRAM)(CONSTANT_STRING, CONSTANT_RXSTRING *, RXSTRING *);

#define REXXTRANSLATEINSTOREPROGRAM RexxTranslateInstoreProgram


char *REXXENTRY RexxGetVersionInformation(void);

typedef char *(REXXENTRY *PFNGETVERSIONINFORMATION)(void);

#define REXXGETVERSIONINFORMATON RexxGetVersionInformation


/*----------------------------------------------------------------------------*/
/***    Subcommand Interface                                                  */
/*----------------------------------------------------------------------------*/

/* This typedef simplifies coding of a Subcommand handler.           */
typedef RexxReturnCode REXXENTRY RexxSubcomHandler(PCONSTRXSTRING,
                                unsigned short *,
                                PRXSTRING);

/***   RexxRegisterSubcomDll -- Register a DLL entry point           */
/***   as a Subcommand handler */

RexxReturnCode REXXENTRY RexxRegisterSubcomDll (
         CONSTANT_STRING,                      /* Name of subcom handler     */
         CONSTANT_STRING,                      /* Name of DLL                */
         CONSTANT_STRING,                      /* Name of procedure in DLL   */
         CONSTANT_STRING,                      /* User area                  */
         size_t );                             /* Drop authority.            */
typedef RexxReturnCode (REXXENTRY *PFNREXXREGISTERSUBCOMDLL)(CONSTANT_STRING, CONSTANT_STRING, CONSTANT_STRING,
                                                    char *, size_t);
#define REXXREGISTERSUBCOMDLL  RexxRegisterSubcomDll


/***   RexxRegisterSubcomExe -- Register an EXE entry point          */
/***   as a Subcommand handler */

RexxReturnCode REXXENTRY RexxRegisterSubcomExe (
         CONSTANT_STRING,              /* Name of subcom handler     */
         REXXPFN,                      /* address of handler in EXE  */
         CONSTANT_STRING);             /* User area                  */
typedef RexxReturnCode (REXXENTRY *PFNREXXREGISTERSUBCOMEXE)(CONSTANT_STRING, REXXPFN, char *);
#define REXXREGISTERSUBCOMEXE  RexxRegisterSubcomExe


/***    RexxQuerySubcom - Query an environment for Existance */

RexxReturnCode REXXENTRY RexxQuerySubcom(
         CONSTANT_STRING,              /* Name of the Environment    */
         CONSTANT_STRING,              /* DLL Module Name            */
         unsigned short *,             /* Stor for existance code    */
         char *);                      /* Stor for user word         */
typedef RexxReturnCode (REXXENTRY *PFNREXXQUERYSUBCOM)(CONSTANT_STRING, CONSTANT_STRING, unsigned short *,
                                              char *);
#define REXXQUERYSUBCOM  RexxQuerySubcom


/***    RexxDeregisterSubcom - Drop registration of a Subcommand     */
/***    environment */

RexxReturnCode REXXENTRY RexxDeregisterSubcom(
         CONSTANT_STRING,                      /* Name of the Environment    */
         CONSTANT_STRING);                     /* DLL Module Name            */
typedef RexxReturnCode (REXXENTRY *PFNREXXDEREGISTERSUBCOM)(CONSTANT_STRING, CONSTANT_STRING);
#define REXXDEREGISTERSUBCOM  RexxDeregisterSubcom


/*----------------------------------------------------------------------------*/
/***    Shared Variable Pool Interface                                        */
/*----------------------------------------------------------------------------*/

/***    RexxVariablePool - Request Variable Pool Service */

RexxReturnCode REXXENTRY RexxVariablePool(
         PSHVBLOCK);                  /* Pointer to list of SHVBLOCKs*/
typedef RexxReturnCode (REXXENTRY *PFNREXXVARIABLEPOOL)(PSHVBLOCK);
#define REXXVARIABLEPOOL  RexxVariablePool


/*----------------------------------------------------------------------------*/
/***    External Function Interface                                           */
/*----------------------------------------------------------------------------*/

/* This typedef simplifies coding of an External Function.           */

typedef size_t REXXENTRY RexxRoutineHandler(CONSTANT_STRING,
                                  size_t,
                                  PCONSTRXSTRING,
                                  CONSTANT_STRING,
                                  PRXSTRING);

typedef size_t REXXENTRY RexxFunctionHandler(CONSTANT_STRING,
                                  size_t,
                                  PCONSTRXSTRING,
                                  CONSTANT_STRING,
                                  PRXSTRING);

/***    RexxRegisterFunctionDll - Register a function in the AFT */

RexxReturnCode REXXENTRY RexxRegisterFunctionDll (
        CONSTANT_STRING,                       /* Name of function to add    */
        CONSTANT_STRING,                       /* Dll file name (if in dll)  */
        CONSTANT_STRING);                      /* Entry in dll               */
typedef RexxReturnCode (REXXENTRY *PFNREXXREGISTERFUNCTIONDLL)(CONSTANT_STRING, CONSTANT_STRING, CONSTANT_STRING);
#define REXXREGISTERFUNCTIONDLL  RexxRegisterFunctionDll


/***    RexxRegisterFunctionExe - Register a function in the AFT */

RexxReturnCode REXXENTRY RexxRegisterFunctionExe (
        CONSTANT_STRING,               /* Name of function to add    */
        REXXPFN);                      /* Entry point in EXE         */
typedef RexxReturnCode (REXXENTRY *PFNREXXREGISTERFUNCTIONEXE)(CONSTANT_STRING, REXXPFN);
#define REXXREGISTERFUNCTIONEXE  RexxRegisterFunctionExe


/***    RexxDeregisterFunction - Delete a function from the AFT */

RexxReturnCode REXXENTRY RexxDeregisterFunction (
        CONSTANT_STRING );                      /* Name of function to remove */
typedef RexxReturnCode (REXXENTRY *PFNREXXDEREGISTERFUNCTION)(CONSTANT_STRING);
#define REXXDEREGISTERFUNCTION  RexxDeregisterFunction


/***    RexxQueryFunction - Scan the AFT for a function */

RexxReturnCode REXXENTRY RexxQueryFunction (
        CONSTANT_STRING );                      /* Name of function to find   */
typedef RexxReturnCode (REXXENTRY *PFNREXXQUERYFUNCTION)(CONSTANT_STRING);
#define REXXQUERYFUNCTION  RexxQueryFunction


/*----------------------------------------------------------------------------*/
/***   System Exits                                                           */
/*----------------------------------------------------------------------------*/

/***    Subfunction RXFNCCAL - External Function Calls */

typedef  struct _RXFNC_FLAGS {          /* fl */
   unsigned rxfferr  : 1;              /* Invalid call to routine.   */
   unsigned rxffnfnd : 1;              /* Function not found.        */
   unsigned rxffsub  : 1;              /* Called as a subroutine     */
}  RXFNC_FLAGS ;

typedef  struct _RXFNCCAL_PARM {        /* fnc */
   RXFNC_FLAGS       rxfnc_flags ;     /* function flags             */
   CONSTANT_STRING   rxfnc_name;       /* Pointer to function name.  */
   unsigned short    rxfnc_namel;      /* Length of function name.   */
   CONSTANT_STRING   rxfnc_que;        /* Current queue name.        */
   unsigned short    rxfnc_quel;       /* Length of queue name.      */
   unsigned short    rxfnc_argc;       /* Number of args in list.    */
   PCONSTRXSTRING    rxfnc_argv;       /* Pointer to argument list.  */
   RXSTRING          rxfnc_retc;       /* Return value.              */
}  RXFNCCAL_PARM;


/***    Subfunction RXOFNCCAL - Object valued external function call */

typedef  struct _RXOFNC_FLAGS {        /* fl */
   unsigned rxfferr  : 1;              /* Invalid call to routine.   */
   unsigned rxffnfnd : 1;              /* Function not found.        */
   unsigned rxffsub  : 1;              /* Called as a subroutine     */
}  RXOFNC_FLAGS ;

typedef  struct _RXOFNCCAL_PARM {      /* fnc */
   RXOFNC_FLAGS      rxfnc_flags ;     /* function flags             */
   CONSTANT_RXSTRING rxfnc_name;       // the called function name
   size_t            rxfnc_argc;       /* Number of args in list.    */
   RexxObjectPtr    *rxfnc_argv;       /* Pointer to argument list.  */
   RexxObjectPtr     rxfnc_retc;       /* Return value.              */
}  RXOFNCCAL_PARM;



/***    Subfunction RXEXFCAL - Scripting Function Calls */

typedef  struct _RXEXF_FLAGS {          /* fl */
   unsigned rxfferr  : 1;              /* Invalid call to routine.   */
   unsigned rxffnfnd : 1;              /* Function not found.        */
   unsigned rxffsub  : 1;              /* Called as a subroutine     */
}  RXEXF_FLAGS ;

typedef  struct _RXEXFCAL_PARM {        /* fnc */
   RXEXF_FLAGS       rxfnc_flags ;     /* function flags             */
   CONSTANT_RXSTRING rxfnc_name;       // the called function name
   size_t            rxfnc_argc;       /* Number of args in list.    */
   RexxObjectPtr    *rxfnc_argv;       /* Pointer to argument list.  */
   RexxObjectPtr     rxfnc_retc;       /* Return value.              */
}  RXEXFCAL_PARM;

/***    Subfunction RXCMDHST -- Process Host Commands     */

typedef  struct _RXCMD_FLAGS {          /* fl */
   unsigned rxfcfail : 1;              /* Command failed.            */
   unsigned rxfcerr  : 1;              /* Command ERROR occurred.    */
}  RXCMD_FLAGS;

typedef  struct _RXCMDHST_PARM {        /* rx */
   RXCMD_FLAGS       rxcmd_flags;      /* error/failure flags        */
   CONSTANT_STRING   rxcmd_address;    /* Pointer to address name.   */
   unsigned short    rxcmd_addressl;   /* Length of address name.    */
   CONSTANT_STRING   rxcmd_dll;        /* dll name for command.      */
   unsigned short    rxcmd_dll_len;    /* Length of dll name.        */
   CONSTANT_RXSTRING rxcmd_command;    /* The command string.        */
   RXSTRING          rxcmd_retc;       /* Pointer to return buffer   */
}  RXCMDHST_PARM;


/***     Subfunction RXMSQPLL -- Pull Entry from Queue */

typedef struct _RXMSQPLL_PARM {        /* pll */
   RXSTRING          rxmsq_retc;       /* Pointer to dequeued entry  */
                                       /* buffer.  User allocated.   */
} RXMSQPLL_PARM;


/***    Subfunction RXMSQPSH -- Push Entry on Queue */
typedef  struct _RXMSQ_FLAGS {          /* fl */
   unsigned rxfmlifo : 1;               /* Stack entry LIFO if set    */
}  RXMSQ_FLAGS;


typedef  struct _RXMSQPSH_PARM {       /* psh */
   RXMSQ_FLAGS       rxmsq_flags;      /* LIFO/FIFO flag             */
   CONSTANT_RXSTRING rxmsq_value;      /* The entry to be pushed.    */
}  RXMSQPSH_PARM;


/***    Subfunction RXMSQSIZ -- Return the Current Queue Size */

typedef struct _RXMSQSIZ_PARM {        /* siz */
   size_t            rxmsq_size;       /* Number of Lines in Queue   */
}  RXMSQSIZ_PARM;


/***    Subfunction RXMSQNAM -- Set Current Queue Name */

typedef struct _RXMSQNAM_PARM {        /* nam */
   RXSTRING     rxmsq_name;            /* RXSTRING containing        */
                                       /* queue name.                */
}  RXMSQNAM_PARM;


/***    Subfunction RXSIOSAY -- Perform SAY Clause */

typedef struct _RXSIOSAY_PARM {        /* say */
   CONSTANT_RXSTRING rxsio_string;     /* String to display.         */
}  RXSIOSAY_PARM;


/***    Subfunction RXSIOTRC -- Write Trace Output */

typedef struct _RXSIOTRC_PARM { /* trcparm */
   CONSTANT_RXSTRING rxsio_string;     /* Trace line to display.     */
}  RXSIOTRC_PARM;


/***    Subfunction RXSIOTRD -- Read Input from the Terminal */

typedef struct _RXSIOTRD_PARM {        /* trd */
   RXSTRING          rxsiotrd_retc;    /* RXSTRING for output.       */
}  RXSIOTRD_PARM;


/***    Subfunction RXSIODTR -- Read Debug Input from the Terminal */

typedef struct _RXSIODTR_PARM {        /* dtr */
   RXSTRING          rxsiodtr_retc;    /* RXSTRING for output.       */
}  RXSIODTR_PARM;


/***    Subfunction RXHSTTST -- Test for HALT Condition */

typedef struct _RXHLT_FLAGS {          /* fl Halt flag               */
   unsigned rxfhhalt : 1;              /* Set if HALT occurred.      */
}  RXHLT_FLAGS;

typedef struct _RXHLTTST_PARM {        /* tst */
   RXHLT_FLAGS rxhlt_flags;            /* Set if HALT occurred       */
}  RXHLTTST_PARM;


/***    Subfunction RXTRCTST -- Test for TRACE Condition */

typedef struct _RXTRC_FLAGS {          /* fl Trace flags             */
   unsigned rxftrace : 1;              /* Set to run external trace. */
}  RXTRC_FLAGS;


typedef struct _RXTRCTST_PARM {        /* tst */
   RXTRC_FLAGS rxtrc_flags;            /* Set to run external trace  */
}  RXTRCTST_PARM;


typedef  struct _RXVARNOVALUE_PARM {   /* var */
   RexxStringObject  variable_name;    // the request variable name
   RexxObjectPtr     value;            // returned variable value
}  RXVARNOVALUE_PARM;


typedef  struct _RXVALCALL_PARM {      /* val */
   RexxStringObject  selector;         // the environment selector name
   RexxStringObject  variable_name;    // the request variable name
   RexxObjectPtr     value;            // returned variable value
}  RXVALCALL_PARM;

/* This typedef simplifies coding of an Exit handler.                */
typedef int REXXENTRY RexxExitHandler(int, int, PEXIT);


/***      RexxRegisterExitDll - Register a system exit. */

RexxReturnCode REXXENTRY RexxRegisterExitDll (
         CONSTANT_STRING,              /* Name of the exit handler   */
         CONSTANT_STRING,              /* Name of the DLL            */
         CONSTANT_STRING,              /* Name of the procedure      */
         CONSTANT_STRING,              /* User area                  */
         size_t);                      /* Drop authority             */
typedef RexxReturnCode (REXXENTRY *PFNREXXREGISTEREXITDLL)(CONSTANT_STRING, CONSTANT_STRING, CONSTANT_STRING,
                                                  char *, size_t);
#define REXXREGISTEREXITDLL  RexxRegisterExitDll


/***      RexxRegisterExitExe - Register a system exit. */

RexxReturnCode REXXENTRY RexxRegisterExitExe (
         CONSTANT_STRING,              /* Name of the exit handler   */
         REXXPFN,                      /* Address of exit handler    */
         CONSTANT_STRING);             /* User area                  */
typedef RexxReturnCode (REXXENTRY *PFNREXXREGISTEREXITEXE)(CONSTANT_STRING, REXXPFN, char *);
#define REXXREGISTEREXITEXE  RexxRegisterExitExe


/***    RexxDeregisterExit - Drop registration of a system exit. */

RexxReturnCode REXXENTRY RexxDeregisterExit (
         CONSTANT_STRING,                       /* Exit name                  */
         CONSTANT_STRING) ;                     /* DLL module name            */
typedef RexxReturnCode (REXXENTRY *PFNREXXDEREGISTEREXIT)(CONSTANT_STRING, CONSTANT_STRING);
#define REXXDEREGISTEREXIT  RexxDeregisterExit


/***    RexxQueryExit - Query an exit for existance. */

RexxReturnCode REXXENTRY RexxQueryExit (
         CONSTANT_STRING,              /* Exit name                  */
         CONSTANT_STRING,              /* DLL Module name.           */
         unsigned short *,             /* Existance flag.            */
         char * );                     /* User data.                 */
typedef RexxReturnCode (REXXENTRY *PFNREXXQUERYEXIT)(CONSTANT_STRING, CONSTANT_STRING, unsigned short *, char *);
#define REXXQUERYEXIT  RexxQueryExit


/*----------------------------------------------------------------------------*/
/***    Asynchronous Request Interface                                        */
/*----------------------------------------------------------------------------*/

/***    RexxSetHalt - Request Program Halt */

RexxReturnCode REXXENTRY RexxSetHalt(
         process_id_t,                /* Process Id                  */
         thread_id_t);                /* Thread Id                   */
typedef RexxReturnCode (REXXENTRY *PFNREXXSETHALT)(process_id_t, thread_id_t);
#define REXXSETHALT  RexxSetHalt


/***    RexxSetTrace - Request Program Trace */

RexxReturnCode REXXENTRY RexxSetTrace(
         process_id_t,                /* Process Id                  */
         thread_id_t);                /* Thread Id                   */
typedef RexxReturnCode (REXXENTRY *PFNREXXSETTRACE)(process_id_t, thread_id_t);
#define REXXSETTRACE  RexxSetTrace


/***    RexxResetTrace - Turn Off Program Trace */

RexxReturnCode REXXENTRY RexxResetTrace(
         process_id_t,                /* Process Id                  */
         thread_id_t);                /* Thread Id                   */
typedef RexxReturnCode (REXXENTRY *PFNREXXRESETTRACE)(process_id_t, thread_id_t);
#define REXXRESETTRACE  RexxResetTrace


/*----------------------------------------------------------------------------*/
/***    Macro Space Interface                                                 */
/*----------------------------------------------------------------------------*/

/***    RexxAddMacro - Register a function in the Macro Space        */

RexxReturnCode REXXENTRY RexxAddMacro(
         CONSTANT_STRING,              /* Function to add or change   */
         CONSTANT_STRING,              /* Name of file to get function*/
         size_t);                      /* Flag indicating search pos  */
typedef RexxReturnCode (REXXENTRY *PFNREXXADDMACRO)(CONSTANT_STRING, CONSTANT_STRING, size_t);
#define REXXADDMACRO  RexxAddMacro


/***    RexxDropMacro - Remove a function from the Macro Space       */

RexxReturnCode REXXENTRY RexxDropMacro (
         CONSTANT_STRING);                      /* Name of function to remove */
typedef RexxReturnCode (REXXENTRY *PFNREXXDROPMACRO)(CONSTANT_STRING);
#define REXXDROPMACRO  RexxDropMacro


/***    RexxSaveMacroSpace - Save Macro Space functions to a file    */

RexxReturnCode REXXENTRY RexxSaveMacroSpace (
         size_t,                              /* Argument count (0==save all)*/
         CONSTANT_STRING *,                   /* List of funct names to save */
         CONSTANT_STRING);                    /* File to save functions in   */
typedef RexxReturnCode (REXXENTRY * PFNREXXSAVEMACROSPACE)(size_t, CONSTANT_STRING *, CONSTANT_STRING);
#define REXXSAVEMACROSPACE  RexxSaveMacroSpace


/***    RexxLoadMacroSpace - Load Macro Space functions from a file  */

RexxReturnCode REXXENTRY RexxLoadMacroSpace (
         size_t,                              /* Argument count (0==load all)*/
         CONSTANT_STRING *,                   /* List of funct names to load */
         CONSTANT_STRING);                    /* File to load functions from */
typedef RexxReturnCode (REXXENTRY *PFNREXXLOADMACROSPACE)(size_t, CONSTANT_STRING *, CONSTANT_STRING);
#define REXXLOADMACROSPACE  RexxLoadMacroSpace


/***    RexxQueryMacro - Find a function's search-order position     */

RexxReturnCode REXXENTRY RexxQueryMacro (
         CONSTANT_STRING,                      /* Function to search for      */
         unsigned short * );                   /* Ptr for position flag return*/
typedef RexxReturnCode (REXXENTRY *PFNREXXQUERYMACRO)(CONSTANT_STRING, unsigned short *);
#define REXXQUERYMACRO  RexxQueryMacro


/***    RexxReorderMacro - Change a function's search-order          */
/***                            position                             */

RexxReturnCode REXXENTRY RexxReorderMacro(
         CONSTANT_STRING,                     /* Name of funct change order  */
         size_t);                             /* New position for function   */
typedef RexxReturnCode (REXXENTRY *PFNREXXREORDERMACRO)(CONSTANT_STRING, size_t);
#define REXXREORDERMACRO  RexxReorderMacro


/***    RexxClearMacroSpace - Remove all functions from a MacroSpace */

RexxReturnCode REXXENTRY RexxClearMacroSpace(
         void );                      /* No Arguments.               */
typedef RexxReturnCode (REXXENTRY *PFNREXXCLEARMACROSPACE)(void);
#define REXXCLEARMACROSPACE  RexxClearMacroSpace


/*----------------------------------------------------------------------------*/
/***    Queing Services                                                       */
/*----------------------------------------------------------------------------*/

#define MAX_QUEUE_NAME_LENGTH 250

/***    RexxCreateQueue - Create an External Data Queue */

RexxReturnCode REXXENTRY RexxCreateQueue (
        char *,                                /* Name of queue created       */
        size_t,                                /* Size of buf for ret name    */
        CONSTANT_STRING,                       /* Requested name for queue    */
        size_t *);                             /* Duplicate name flag.        */
typedef RexxReturnCode (REXXENTRY *PFNREXXCREATEQUEUE)(char *, size_t, CONSTANT_STRING, size_t);

/***    RexxOpenQueue - Create a named external queue, if necessary */

RexxReturnCode REXXENTRY RexxOpenQueue (
        CONSTANT_STRING,                       /* Requested name for queue    */
        size_t *);                             /* Flag for already created queue */
typedef RexxReturnCode (REXXENTRY *PFNREXXOPENQUEUE)(CONSTANT_STRING, size_t);


/***    QueueClassExists - Check for the existance of an external data queue */

RexxReturnCode REXXENTRY RexxQueueExists (
        CONSTANT_STRING);                      /* Name of queue to be deleted */
typedef RexxReturnCode (REXXENTRY *PFNREXXQUEUEEXISTS)(CONSTANT_STRING);

/***    RexxDeleteQueue - Delete an External Data Queue */

RexxReturnCode REXXENTRY RexxDeleteQueue (
        CONSTANT_STRING);                      /* Name of queue to be deleted */
typedef RexxReturnCode (REXXENTRY *PFNREXXDELETEQUEUE)(CONSTANT_STRING);


/*** RexxQueryQueue - Query an External Data Queue for number of      */
/***                  entries                                         */

RexxReturnCode REXXENTRY RexxQueryQueue (
        CONSTANT_STRING,                       /* Name of queue to query      */
        size_t *);                             /* Place to put element count  */
typedef RexxReturnCode (REXXENTRY *PFNREXXQUERYQUEUE)(CONSTANT_STRING, size_t *);


/***    RexxAddQueue - Add an entry to an External Data Queue */

RexxReturnCode REXXENTRY RexxAddQueue (
        CONSTANT_STRING,                       /* Name of queue to add to     */
        PCONSTRXSTRING,                        /* Data string to add          */
        size_t);                               /* Queue type (FIFO|LIFO)      */
typedef RexxReturnCode (REXXENTRY *PFNREXXADDQUEUE)(CONSTANT_STRING, PCONSTRXSTRING, size_t);

/***    RexxPullFromQueue - Retrieve data from an External Data Queue */
RexxReturnCode REXXENTRY RexxPullFromQueue (
        CONSTANT_STRING,                       /* Name of queue to read from  */
        PRXSTRING,                             /* RXSTRING to receive data    */
        RexxQueueTime *,                       /* Stor for data date/time     */
        size_t);                               /* wait status (WAIT|NOWAIT)   */
typedef RexxReturnCode (REXXENTRY *PFNREXXPULLFROMQUEUE)(CONSTANT_STRING, PRXSTRING, RexxQueueTime *,
                                           size_t);

/***    RexxClearQueue - Clear all lines in a queue */

RexxReturnCode REXXENTRY RexxClearQueue (
        CONSTANT_STRING );                         /* Name of queue to be deleted */
typedef RexxReturnCode (REXXENTRY *PFNREXXCLEARQUEUE)(CONSTANT_STRING);


#include "rexxplatformapis.h"

/*----------------------------------------------------------------------------*/
/***    Memory Allocation Services                                            */
/*----------------------------------------------------------------------------*/

/***   RexxAllocateMemory            */

void *REXXENTRY RexxAllocateMemory(
                   size_t);                    /* number of bytes to allocate */
typedef void *(REXXENTRY *PFNREXXALLOCATEMEMORY)(size_t );


/***   RexxFreeMemory                */

RexxReturnCode REXXENTRY RexxFreeMemory(
                   void *);  /* pointer to the memory returned by    */
                             /* RexxAllocateMemory                   */
typedef RexxReturnCode (REXXENTRY *PFNREXXFREEMEMORY)(void *);


END_EXTERN_C()

#endif /* REXXSAA_INCLUDED */

