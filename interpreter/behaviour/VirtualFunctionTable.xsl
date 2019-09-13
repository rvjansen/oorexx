<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>

<xsl:template match="Classes">
<xsl:text>/*----------------------------------------------------------------------------*/
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
/******************************************************************************/
/* REXX Kernel                                                                */
/*                                                                            */
/* Build the table of virtual functions assigned to Rexx class instances      */
/*                                                                            */
/******************************************************************************/

/* -------------------------------------------------------------------------- */
/* --            ==================================================        -- */
/* --            DO NOT CHANGE THIS FILE, ALL CHANGES WILL BE LOST!        -- */
/* --            ==================================================        -- */
/* -------------------------------------------------------------------------- */

#include &lt;new&gt;
#include "RexxCore.h"

   </xsl:text>
<xsl:variable name="unique-list" select="//Class"/>

<xsl:for-each select="$unique-list">
<xsl:if test="not(@include = (preceding::*/@include))">
<xsl:text>
#include "</xsl:text><xsl:value-of select="@include"/><xsl:text>"</xsl:text>
</xsl:if>
</xsl:for-each>
<xsl:text>


void *MemoryObject::virtualFunctionTable[T_Last_Class_Type + 1] = {NULL};

/******************************************************************************/
/* Function:  This small function is necessary to void optimizer problems on  */
/*            some versions of GCC.  The optimizer appears to keep storing    */
/*            the same value in the VFT rather than picking up the new VFT    */
/*            for each class.  Making this a separate routine avoids this.    */
/******************************************************************************/
void *getVftPointer(void *loc)
{
    return *((void **)loc);
}

void MemoryObject::buildVirtualFunctionTable()
/******************************************************************************/
/* Function:  This routine will build an array of the virtualFunctions        */
/*            There will be one for each Class.                               */
/******************************************************************************/
{
    uintptr_t objectBuffer[256];       /* buffer for each object            */
    volatile void *objectPtr;

    void *objectLoc = objectBuffer;
    // instantiate an instance of each class into the buffer and
    // grab the resulting virtual function table
   </xsl:text>

   <xsl:for-each select="Exported/Class">
   <xsl:text>
   objectPtr = ::new (objectLoc) </xsl:text><xsl:value-of select="@class"/><xsl:text>(RESTOREIMAGE);
   virtualFunctionTable[T_</xsl:text><xsl:value-of select="@id"/><xsl:text>] = getVftPointer(objectLoc);
   </xsl:text>
   <xsl:if test="@classclass">
   <xsl:text>
   objectPtr = ::new (objectLoc) </xsl:text><xsl:value-of select="@classclass"/><xsl:text>(RESTOREIMAGE);</xsl:text>
   </xsl:if>
   <xsl:if test="not(@classclass)">
   <xsl:text>
   objectPtr = ::new (objectLoc) RexxClass(RESTOREIMAGE);</xsl:text>
   </xsl:if>
   <xsl:text>
   virtualFunctionTable[T_</xsl:text><xsl:value-of select="@id"/><xsl:text>Class] = getVftPointer(objectLoc);
   </xsl:text>

   </xsl:for-each>

   <xsl:for-each select="Internal/Class">
   <xsl:text>
   objectPtr = ::new (objectLoc) </xsl:text><xsl:value-of select="@class"/><xsl:text>(RESTOREIMAGE);
   virtualFunctionTable[T_</xsl:text><xsl:value-of select="@id"/><xsl:text>] = getVftPointer(objectLoc);
   </xsl:text>
   </xsl:for-each>

   <xsl:for-each select="Transient/Class">
   <xsl:if test="@objectvirtual">
   <xsl:text>
   objectPtr = ::new (objectLoc) RexxObject(RESTOREIMAGE);
   virtualFunctionTable[T_</xsl:text><xsl:value-of select="@id"/><xsl:text>] = getVftPointer(objectLoc);
   </xsl:text>
   </xsl:if>
   <xsl:if test="not(@objectvirtual)">
   <xsl:text>
   objectPtr = ::new (objectLoc) </xsl:text><xsl:value-of select="@class"/><xsl:text>(RESTOREIMAGE);
   virtualFunctionTable[T_</xsl:text><xsl:value-of select="@id"/><xsl:text>] = getVftPointer(objectLoc);
   </xsl:text>
   </xsl:if>
   </xsl:for-each>

   <xsl:text>
};


/* --            ==================================================        -- */
/* --            DO NOT CHANGE THIS FILE, ALL CHANGES WILL BE LOST!        -- */
/* --            ==================================================        -- */
/* -------------------------------------------------------------------------- */
</xsl:text>
</xsl:template>
<xsl:template match="CopyRight"></xsl:template>
</xsl:stylesheet>
