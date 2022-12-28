/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2020 Rexx Language Association. All rights reserved.    */
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
/* REXX Kernel                                                                */
/*                                                                            */
/* Stream processing (stream oriented file systems)                           */
/*                                                                            */
/******************************************************************************/

#include "oorexxapi.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "SysFileSystem.hpp"
#include "ExternalFileBuffer.hpp"

/**
 * Return the file name separator used by the file system.
 */
RexxMethod0(CSTRING, file_separator)
{
    return SysFileSystem::getSeparator();
}

/**
 * Return the separator used for file search paths
 */
RexxMethod0(CSTRING, file_path_separator)
{
    return SysFileSystem::getPathSeparator();
}


/**
 * Return the file system case sensitivity
 */
RexxMethod0(logical_t, file_case_sensitive)
{
    return SysFileSystem::isCaseSensitive();
}


/**
 * Return the case sensitivity of a file
 */
RexxMethod1(logical_t, this_file_case_sensitive, CSTRING, fileName)
{
    // Ask the file system for an answer
    return SysFileSystem::isCaseSensitive(fileName);
}


/**
 * Return whether file can be opened for reading
 */
RexxMethod1(logical_t, file_can_read, CSTRING, name)
{
    return SysFileSystem::exists(name) && SysFileSystem::canRead(name);
}


/**
 * Return whether file can be opened for writing
 */
RexxMethod1(logical_t, file_can_write, CSTRING, name)
{
    return SysFileSystem::exists(name) && SysFileSystem::canWrite(name);
}


/**
 * Return the list of file system root elements.
 */
RexxMethod0(RexxArrayObject, file_list_roots)
{
    MethodFileNameBuffer rootBuffer(context);

    int count = SysFileSystem::getRoots(rootBuffer);

    const char *roots = rootBuffer;

    RexxArrayObject result = context->NewArray(count);
    for (int i = 0; i < count; i++)
    {
        context->ArrayAppendString(result, roots, strlen(roots));
        roots += strlen(roots) + 1;
    }

    return result;
}


/**
 * Create a fully-qualified path name for a file.
 */
RexxMethod1(RexxStringObject, file_qualify, CSTRING, name)
{
    QualifiedName qualifiedName(name);

    return context->String(qualifiedName);
}


/**
 * Test if the file exists
 */
RexxMethod1(logical_t, file_exists, CSTRING, name)
{
    return SysFileSystem::exists(name);
}


/**
 * Delete a file
 */
RexxMethod1(logical_t, file_delete_file, CSTRING, name)
{
    return SysFileSystem::deleteFile(name) == 0;
}


/**
 * Delete a directory
 */
RexxMethod1(logical_t, file_delete_directory, CSTRING, name)
{
    return SysFileSystem::deleteDirectory(name) == 0;
}


/**
 * Tests if the file is a directory
 */
RexxMethod1(logical_t, file_isDirectory, CSTRING, name)
{
    return SysFileSystem::isDirectory(name);
}


/**
 * Tests if the file is a file
 */
RexxMethod1(logical_t, file_isFile, CSTRING, name)
{
    return SysFileSystem::isFile(name);
}


/**
 * Tests if the file hidden
 */
RexxMethod1(logical_t, file_isHidden, CSTRING, name)
{
    return SysFileSystem::isHidden(name);
}


/**
 * Return the last modified date as a Ticks time value.
 */
RexxMethod1(int64_t, file_get_last_modified, CSTRING, name)
{
    return SysFileSystem::getLastModifiedDate(name);
}


/**
 * Return the last modified date as a Ticks time value.
 */
RexxMethod2(logical_t, file_set_last_modified, CSTRING, name, int64_t, time)
{
    return SysFileSystem::setLastModifiedDate(name, time);
}


/**
 * Return the last access date as a Ticks time value.
 */
RexxMethod1(int64_t, file_get_last_accessed, CSTRING, name)
{
    return SysFileSystem::getLastAccessDate(name);
}


/**
 * Return the last modified date as a Ticks time value.
 */
RexxMethod2(logical_t, file_set_last_accessed, CSTRING, name, int64_t, time)
{
    return SysFileSystem::setLastAccessDate(name, time);
}


/**
 * Set the read-only flag for the target file
 */
RexxMethod1(logical_t, file_set_read_only, CSTRING, name)
{
    return SysFileSystem::setFileReadOnly(name);
}


/**
 * Set the read-only flag for the target file
 */
RexxMethod1(logical_t, file_set_writable, CSTRING, name)
{
    return SysFileSystem::setFileWritable(name);
}



/**
 * Return the last modified date as a Ticks time value.
 */
RexxMethod1(uint64_t, file_length, CSTRING, name)
{
    return SysFileSystem::getFileLength(name);
}


/**
 * Get a list of the file children for a directory.
 */
RexxMethod1(RexxObjectPtr, file_list, CSTRING, name)
{
    if (!SysFileSystem::isDirectory(name))
    {
        return context->Nil();
    }

    // create an empty array to start
    RexxArrayObject result = context->NewArray(0);

    MethodFileNameBuffer buffer(context);
    MethodFileNameBuffer nextFile(context);

    SysFileIterator iterator(name, NULL, buffer);
    SysFileIterator::FileAttributes attributes;

    while (iterator.hasNext())
    {
        iterator.next(nextFile, attributes);
        // don't include the "." and ".." directories in this list
        if (nextFile != "." && nextFile != "..")
        {
            context->ArrayAppendString(result, nextFile, nextFile.length());
        }
    }

    return result;
}


/**
 * Make a directory instance
 */
RexxMethod1(logical_t, file_make_dir, CSTRING, name)
{
    return SysFileSystem::makeDirectory(name);
}


/**
 * Rename a file.
 */
RexxMethod2(logical_t, file_rename, CSTRING, fromName, CSTRING, toName)
{
    return SysFileSystem::moveFile(fromName, toName) == 0;
}


/**
 * Return the temporary directory.
 */
RexxMethod0(RexxStringObject, file_temporary_path)
{
    MethodFileNameBuffer temporary(context);

    SysFileSystem::getTemporaryPath(temporary);
    return context->String(temporary);
}


/**
 * Return the file system case sensitivity section
 */
RexxMethod2(RexxObjectPtr, file_search_path_impl, CSTRING, file, CSTRING, path)
{
    MethodFileNameBuffer found(context);

    if (SysFileSystem::searchPath(file, path, found))
    {
        return context->String(found);
    }
    return context->Nil();

}



