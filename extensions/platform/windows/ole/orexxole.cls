/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2022 Rexx Language Association. All rights reserved.    */
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
/* */

-- NOTE:  We only add these to the environment, they are not part of the
-- REXX package.
.environment~setentry("OLEOBJECT", .OLEObject)
.environment~setentry("OLEVARIANT", .OLEVariant)

::CLASS 'OLEObject'
::METHOD New CLASS
  Signal On Syntax

  If Arg() < 1 Then
    Raise Syntax 93.903 Array(1) Description "A CLSID, ProgID, or IDispatch pointer is required to create an OLEObject."
  Forward Class(Super)

Syntax:
  Raise Propagate

::METHOD init EXTERNAL "LIBRARY OREXXOLE OLEObject_Init"
::METHOD uninit EXTERNAL "LIBRARY OREXXOLE OLEObject_Uninit"
::METHOD addRef private EXTERNAL "LIBRARY OREXXOLE OLEObject_addRef_pvt"
::METHOD hasOLEMethod EXTERNAL "LIBRARY OREXXOLE OLEObject_hasOLEMethod"
::METHOD unknown EXTERNAL "LIBRARY OREXXOLE OLEObject_Unknown"
::METHOD request EXTERNAL "LIBRARY OREXXOLE OLEObject_Request"
::METHOD getConstant EXTERNAL "LIBRARY OREXXOLE OLEObject_GetConst"
::METHOD "!GETVAR" EXTERNAL "LIBRARY OREXXOLE OLEObject_GetVar"
::METHOD getKnownMethods EXTERNAL "LIBRARY OREXXOLE OLEObject_GetKnownMethods"
::METHOD getKnownEvents EXTERNAL "LIBRARY OREXXOLE OLEObject_GetKnownEvents"
::METHOD isConnectable EXTERNAL "LIBRARY OREXXOLE OLEObject_isConnectable"
::METHOD isConnected EXTERNAL "LIBRARY OREXXOLE OLEObject_isConnected"
::METHOD connectEvents EXTERNAL "LIBRARY OREXXOLE OLEObject_connectEvents"
::METHOD disconnectEvents EXTERNAL "LIBRARY OREXXOLE OLEObject_disconnectEvents"
::METHOD removeEventHandler EXTERNAL "LIBRARY OREXXOLE OLEObject_removeEventHandler"
--::METHOD GETKNOWNMETHODS CLASS EXTERNAL "LIBRARY OREXXOLE OLEObject_GetKnownMethods_Class"
::METHOD GETOBJECT CLASS EXTERNAL "LIBRARY OREXXOLE OLEObject_GetObject_Class"

::method send
  if self~hasOLEMethod("SEND") then
    forward message("UNKNOWN") ARRAY("SEND", arg(1, 'A'))
  else
    forward class (super)

::method sendWith
  if self~hasOLEMethod("SENDWITH") then
    forward message("UNKNOWN") ARRAY("SENDWITH", arg(1, 'A'))
  else
    forward class (super)

::method copy
  if self~hasOLEMethod("COPY") then do
    forward message("UNKNOWN") ARRAY("COPY", arg(1, 'A'))
  end
  else do
    forward class (super) continue
    oleObj = result
    if oleObj~isInstanceOf(.oleObject) then do
      -- We have a copy of an OLEObject, including a copy of its dispatch pointer.
      -- The dispatch pointer needs to have its reference count increased by 1.
      oleObj~addRef
    end
    return oleObj
  end

::method class
  if self~hasOLEMethod("CLASS") then
    forward message("UNKNOWN") ARRAY("CLASS", arg(1, 'A'))
  else
    forward class (super)

::ATTRIBUTE "PROGID" GET
  expose !progid progid_!
  if var("!PROGID") then return !progid
  if var("PROGID_!") then return progid_! -- derived from IDISPATCH
  return .nil

::ATTRIBUTE "CLSID"     GET
  expose !clsid clsid_!
  if var("!CLSID") then return !clsid
  if var("CLSID_!") then return clsid_!  -- derived from IDISPATCH
  return .nil

::METHOD "!OLEOBJECT" ATTRIBUTE

::METHOD addEventMethod
  use strict arg name, method
  self~setMethod(name, method, "object")

::METHOD removeEventMethod
  use strict arg name
  self~unsetMethod(name)

::METHOD GETOUTPARAMETERS
  expose !outArray
  return !outArray

::METHOD DISPATCH
use arg name
return self~unknown(name, arg(2,'A'))

::CLASS 'OLEVariant'
::METHOD new class
  signal on syntax

  if arg(1, 'O') then
    raise syntax 93.903 array(1) description "A variant value is required to create an OLEVariant."
  if arg() > 3 then
    raise syntax 93.902 array(3) description "The only arguments allowed to create an OLEVariant are: value, type, param flags."
  forward class(super)

syntax:
  raise propagate

::METHOD INIT EXTERNAL "LIBRARY OREXXOLE OLEVariant_Init"

::METHOD "!VARVALUE_=" EXTERNAL "LIBRARY OREXXOLE OLEVariant_VarValueEquals"
::METHOD "!VARVALUE_"
  expose !_VAR_VALUE_
  return !_VAR_VALUE_

::METHOD "!VARTYPE_=" EXTERNAL "LIBRARY OREXXOLE OLEVariant_VarTypeEquals"
::METHOD "!VARTYPE_"
  expose !_VAR_TYPE_STR_
  return !_VAR_TYPE_STR_

::METHOD "!_VT_"
  expose !_VAR_TYPE_
  return !_VAR_TYPE_

::METHOD "!PARAMFLAGS_=" EXTERNAL "LIBRARY OREXXOLE OLEVariant_ParamFlagsEquals"
::METHOD "!PARAMFLAGS_"
  expose !_PARAM_FLAGS_STR_
  return !_PARAM_FLAGS_STR_

::METHOD "!_PFLAGS_"
  expose !_PARAM_FLAGS_
  return !_PARAM_FLAGS_

::METHOD "!OLEVARIANT_" ATTRIBUTE

-- !clearVariant_ is an undocumented method for internal use only.
::METHOD "!CLEARVARIANT_"
  expose !_CLEAR_VARIANT_
  return !_CLEAR_VARIANT_

::METHOD "!CLEARVARIANT_="
  expose !_CLEAR_VARIANT_
  use arg bool
  signal on syntax

  errDescription = "!CLEARVARIANT_  must be set to .true or .false."
  if arg(1, 'O') then
    raise syntax 93.903 array('bool') description (errDescription)
  if bool <> .true & bool <> .false then
    raise syntax 93.914 array('bool', "[.true,.false]", bool) description (errDescription)

  !_CLEAR_VARIANT_ = (bool == .true)
  return

syntax:
  raise propagate

-- !variantPointer_ is an undocumented method for internal use only.
::METHOD "!VARIANTPOINTER_"
  expose !_VARIANT_PTR_
  return !_VARIANT_PTR_

::METHOD "!VARIANTPOINTER_="
  expose !_VARIANT_PTR_
  use arg pointer
  signal on syntax

  errDescription = "!VARIANTPOINTER_ must be set to a valid pointer value."
  if arg(1, 'O') then
    raise syntax 93.903 array(1) description (errDescription)
  if \ pointer~isA(.Pointer) then
    raise syntax 93.948 array(1, ".Pointer") description (errDescription)

  !_VARIANT_PTR_ = pointer
  return

syntax:
  raise propagate
