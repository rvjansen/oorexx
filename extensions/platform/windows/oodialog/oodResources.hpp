/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2009-2014 Rexx Language Association. All rights reserved.    */
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

#ifndef oodResources_Included
#define oodResources_Included

typedef struct _OODIMAGE
{
    SIZE     size;
    HANDLE   hImage;
    LONG     type;
    DWORD    flags;
    DWORD    lastError;
    CSTRING  typeName;
    CSTRING  fileName;   // Not currently used, may change to char[256].

    bool     srcOOD;     // True - comes from ooDialog code using LoadImage(),
                         // False comes from a raw retrieved handle.
    bool     canRelease;
    bool     isValid;
} OODIMAGE, *POODIMAGE;


typedef struct _RESOURCEIMAGE
{
    HMODULE  hMod;
    DWORD    lastError;
    bool     canRelease;
    bool     isValid;
} RESOURCEIMAGE, *PRESOURCEIMAGE;


extern POODIMAGE     rxGetOodImage(RexxMethodContext *, RexxObjectPtr, size_t);
extern RexxObjectPtr rxNewValidImage(RexxMethodContext *, HANDLE, uint8_t, PSIZE, uint32_t, bool);
extern POODIMAGE     rxGetImageIcon(RexxMethodContext *, RexxObjectPtr, size_t);
extern POODIMAGE     rxGetImageCursor(RexxMethodContext *c, RexxObjectPtr o, size_t pos);
extern POODIMAGE     rxGetImageBitmap(RexxMethodContext *c, RexxObjectPtr o, size_t pos);
extern RexxObjectPtr oodGetImageAttribute(RexxMethodContext *, RexxObjectPtr, CSTRING, UINT, WPARAM, uint8_t, oodControl_t);
extern RexxObjectPtr oodSetImageAttribute(RexxMethodContext *, CSTRING, RexxObjectPtr, HWND, HANDLE, uint8_t, oodControl_t);
extern CSTRING       getImageTypeName(uint8_t);
extern HIMAGELIST    rxGetImageList(RexxMethodContext *, RexxObjectPtr, size_t);
extern RexxObjectPtr oodILFromBMP(RexxMethodContext *, HIMAGELIST *, RexxObjectPtr, int, int, HWND);

extern PRESOURCEIMAGE rxGetResourceImage(RexxMethodContext *context, RexxObjectPtr r, size_t argPos);


#define IMAGE_TYPE_LIST        "Bitmap, Icon, Cursor, EnhMetafile"
#define IMAGE_FLAGS_LIST       "DefaultColor, MonoChrome, Color, CopyReturnOrg, CopyDeleteOrg, LoadFromFile, LoadTransparent, DefaultSize, VgaColor, LoadMap3DColors, CreateDibSection, CopyFromResource, or Shared"
#define IMAGELIST_CREATE_LIST  "Mask, Color, ColordDB, Color4, Color8, Color16, Color24, Color32, Palette, Mirror, PerItemMirror, OriginalSize, or HighQualityScale"
#define LOAD_RESOURCE_LIST     "Blend, Blend25, Blend50, Focus, Mask, Normal, Selected, or Transparent"


#endif
