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
/* REXX Kernel                                              SysFile.hpp       */
/*                                                                            */
/* System support for File operations.                                        */
/*                                                                            */
/******************************************************************************/

#ifndef Included_SysFile
#define Included_SysFile

#include "rexxapitypes.h"
#include <fcntl.h>
#if defined(__OpenBSD__)
#include <sys/stat.h>
#endif

// The following define the platform independent open flags
// openFlags argument flags
#define RX_O_RDONLY       O_RDONLY
#define RX_O_WRONLY       O_WRONLY
#define RX_O_RDWR         O_RDWR
#define RX_O_CREAT        O_CREAT
#define RX_O_EXCL         O_EXCL
#define RX_O_TRUNC        O_TRUNC
#define RX_O_APPEND       O_APPEND
// openMode flags
#define RX_SH_DENYWR      0
#define RX_SH_DENYRD      0
#define RX_SH_DENYRW      0
#define RX_SH_DENYNO      0
// shareMode flags
#define RX_S_IWRITE       (S_IWUSR | S_IWGRP | S_IWOTH)
#define RX_S_IREAD        (S_IRUSR | S_IRGRP | S_IROTH)

class SysFile
{
 public:
     SysFile();

     static const int stdinHandle;
     static const int stdoutHandle;
     static const int stderrHandle;

     enum
     {
         DEFAULT_BUFFER_SIZE = 4096,   // default size for buffering
         LINE_POSITIONING_BUFFER = 512 // buffer size for line movement
     };

#define LINE_TERMINATOR "\n"

     bool open(const char *name, int openFlags, int openMode, int shareMode);
     bool open(int handle);
     void reset();
     void setStdIn();
     void setStdOut();
     void setStdErr();
     void setBuffering(bool buffer, size_t length);
     bool close();
     bool flush();
     bool read(char *buf, size_t len, size_t &bytesRead);
     bool write(const char *data, size_t len, size_t &bytesWritten);
     bool putChar(char ch);
     bool ungetc(char ch);
     inline bool getChar(char &ch) { size_t len; return read(&ch, 1, len); }
     bool puts(const char *data, size_t &bytesWritten);
     bool gets(char *buffer, size_t len, size_t &bytesRead);
     bool setPosition(int64_t location, int64_t &position);
     bool seek(int64_t offset, int direction, int64_t &position);
     bool getPosition(int64_t &position);
     bool getSize(int64_t &size);
     bool getSize(const char *name, int64_t &size);
     bool getTimeStamp(const char *&time);
     bool getTimeStamp(const char *name, const char *&time);
     bool putLine(const char *buffer, size_t len, size_t &bytesWritten);
     bool hasData();
     bool countLines(int64_t &count);
     bool countLines(int64_t start, int64_t end, int64_t &lastLine, int64_t &count);
     bool nextLine(size_t &bytesRead);
     bool seekForwardLines(int64_t startPosition, int64_t &lineCount, int64_t &endPosition);

     inline bool isTransient() { return transient; }
     inline bool isDevice() { return device; }
     inline bool isReadable() { return readable; }
     inline bool isWriteable() { return writeable; }
     inline bool isOpen() { return fileHandle != -1; }
     inline bool isStdIn() { return fileHandle == stdinHandle; }

     inline bool error() { return errInfo != 0; }
     inline int  errorInfo() { return errInfo; }
     inline void clearErrors() { errInfo = 0; }
     inline bool atEof() { return !hasData(); }
     inline bool hasBufferedInput() { return buffered && (bufferedInput > bufferPosition); }
     inline uintptr_t getHandle() { return (uintptr_t)fileHandle; }

 protected:
     void   getStreamTypeInfo();

     int    fileHandle;      // separate file handle
     int    errInfo;         // last error info
     bool   openedHandle;    // true if we opened the handle.
     int    flags;           // open flag information
     int    mode;            // mode flags
     int    share;           // sharing mode flags
     const char  *filename;  // the input file name
     bool   buffered;        // the buffering indicator
     bool   transient;       // this is a transient stream
     bool   device;          // this stream is a device.
     bool   writeable;       // stream is capable of output
     bool   readable;        // stream is capable in input
     bool   isTTY;           // a keyboard based stream.
     char  *buffer;          // our read/write buffer.
     size_t bufferSize;      // the size of the buffer
     size_t bufferPosition; // current read/write position in buffer
     size_t bufferedInput;   // amount of data in the buffer
     bool   writeBuffered;   // false == read, true == write
     bool   append;          // opened in append mode
     int64_t filePointer;    // current file pointer location
     int    ungetchar;       // a pushed back character value
     bool   fileeof;         // have we reached eof?
};

#endif


