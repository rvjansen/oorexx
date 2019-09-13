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
/*                                                                            */
/* Primitive MutableBuffer Class                                              */
/*                                                                            */
/******************************************************************************/
#include <algorithm>
#include <ctype.h>

#include "RexxCore.h"
#include "StringClass.hpp"
#include "MutableBufferClass.hpp"
#include "ProtectedObject.hpp"
#include "StringUtil.hpp"
#include "MethodArguments.hpp"


// singleton class instance
RexxClass *MutableBuffer::classInstance = OREF_NULL;



/**
 * Create initial class object at bootstrap time.
 */
void MutableBuffer::createInstance()
{
    CLASS_CREATE(MutableBuffer);
}


/**
 * Create a new mutable buffer object from a potential subclass.
 *
 * @param size   The size of the buffer object.
 *
 * @return A new instance of a mutable buffer, with the default class
 *         behaviour.
 */
void *MutableBuffer::operator new(size_t size)
{
    return new_object(size, T_MutableBuffer);
}


/**
 * Allocate a MutableBuffer object from Rexx code.
 *
 * @param args   The pointer to the arrays.
 * @param argc   The count of arguments.
 *
 * @return A new mutable buffer object.
 */
MutableBuffer *MutableBuffer::newRexx(RexxObject **args, size_t argc)
{
    // this class is defined on the object class, but this is actually attached
    // to a class object instance.  Therefore, any use of the this pointer
    // will be touching the wrong data.  Use the classThis pointer for calling
    // any methods on this object from this method.
    RexxClass *classThis = (RexxClass *)this;

    // default string value
    RexxString *string = GlobalNames::NULLSTRING;
    size_t bufferLength = DEFAULT_BUFFER_LENGTH;
    size_t defaultSize = 0;
    // if we have at least one argument, then the first
    // argument is an initial string value
    if (argc >= 1)
    {
        if (args[0] != NULL)
        {
            string = stringArgument(args[0], ARG_ONE);
        }
    }

    // have a minimum buffer size specified
    if (argc >= 2)
    {
        bufferLength = optionalLengthArgument(args[1], DEFAULT_BUFFER_LENGTH, ARG_TWO);
    }

    // remember this as the default
    defaultSize = bufferLength;

    // input string longer than demanded minimum size? expand accordingly
    if (string->getLength() > bufferLength)
    {
        bufferLength = string->getLength();
    }
    // allocate the new object
    Protected<MutableBuffer> newBuffer = new MutableBuffer(bufferLength, defaultSize);

    newBuffer->dataLength = string->getLength();
    // copy the content
    newBuffer->copyData(0, string->getStringData(), string->getLength());

    // handle Rexx class completion
    classThis->completeNewObject(newBuffer, args, argc > 2 ? argc - 2 : 0);
    return newBuffer;
}


/**
 * Default constructor.
 */
MutableBuffer::MutableBuffer()
{
    bufferLength = DEFAULT_BUFFER_LENGTH;   /* save the length of the buffer    */
    defaultSize  = bufferLength;            /* store the default buffer size    */
    // NB:  we clear this before we allocate the new buffer because allocating the
    // new buffer might trigger a garbage collection, causing us to mark bogus
    // reference.
    data = OREF_NULL;
    data = new_buffer(bufferLength);

}


/**
 * Constructor with explicitly set size and default.
 *
 * @param l      Initial length.
 * @param d      The explicit default size.
 */
MutableBuffer::MutableBuffer(size_t l, size_t d)
{
    bufferLength = l;               /* save the length of the buffer    */
    defaultSize  = d;               /* store the default buffer size    */
    // NB: As in the default constructor, we clear this before we allocate the
    // new buffer in case garbage collection is triggered.
    data = OREF_NULL;
    data = new_buffer(bufferLength);
}


/**
 * Normal garbage collection live marking
 *
 * @param liveMark The current live mark.
 */
void MutableBuffer::live(size_t liveMark)
{
    memory_mark(objectVariables);
    memory_mark(data);
}


/**
 * Generalized object marking.
 *
 * @param reason The reason for this live marking operation.
 */
void MutableBuffer::liveGeneral(MarkReason reason)
{
    memory_mark_general(objectVariables);
    memory_mark_general(data);
}


/**
 * Flatten the table contents as part of a saved program.
 *
 * @param envelope The envelope we're flattening into.
 */
void MutableBuffer::flatten(Envelope *envelope)
{
    setUpFlatten(MutableBuffer)

    flattenRef(data);
    flattenRef(objectVariables);

    cleanUpFlatten
}


/**
 * copy an object
 *
 * @return A copy of the object.
 */
RexxInternalObject *MutableBuffer::copy()
{
    // use the base copy method so that the object variables get copied to.
    MutableBuffer *newObj = (MutableBuffer *)RexxObject::copy();

    // copy the buffer...al other values have already been copied.
    newObj->data = new_buffer(bufferLength);
    newObj->copyData(0, data->getData(), bufferLength);
    return newObj;
}


/**
 * Ensure that the MutableBuffer has sufficient space for
 * an append operation.
 *
 * @param addedLength
 *               The additional length required.
 */
void MutableBuffer::ensureCapacity(size_t addedLength)
{
    size_t resultLength = dataLength + addedLength;

    if (resultLength > bufferLength)
    {
        // use the larger of the required length and twice the buffer size as our new size.
        bufferLength = std::max(resultLength, bufferLength * 2);

        // get an expanded buffer
        setField(data, data->expand(bufferLength));
    }
}


/**
 * Set the length of the data in the buffer.  The limit is
 * the current capacity of the buffer.  If the length is
 * extended beyond the current length, the extra characters
 * of the buffer will be filled with nulls.
 *
 * @param newLength The new datalength.  This is capped to the capacity of
 *                  the buffer.
 *
 * @return The actual length the data has been set to.  If the
 *         target length is greater than the capacity, the capacity
 *         value is returned.
 */
size_t MutableBuffer::setDataLength(size_t newLength)
{
    // cap the data length at the capacity
    size_t capacity = getCapacity();
    if (newLength > capacity)
    {
        newLength = capacity;
    }

    size_t oldLength = getLength();
    // set the new buffer length
    dataLength = newLength;
    // do we need to pad?
    if (newLength > oldLength)
    {
        setData(oldLength, '\0', newLength - oldLength);
    }

    return newLength;
}

/**
 * Set the capacity of the buffer.
 *
 * @param newLength The new buffer length
 *
 * @return The pointer to the data area in the buffer.
 */
char *MutableBuffer::setCapacity(size_t newLength)
{
    // if the new length is longer than our current,
    // extend by the delta
    if (newLength > bufferLength)
    {
        ensureCapacity(newLength - bufferLength);
    }
    // return a pointer to the current buffer data
    return getData();
}


/**
 * Return the length of the data in the buffer currently.
 *
 * @return The current length, as an Integer object.
 */
RexxObject *MutableBuffer::lengthRexx()
{
    return new_integer(dataLength);
}


/**
 * Append a value to this buffer.
 *
 * @param obj    The object to append.
 *
 * @return returns the same buffer.
 */
MutableBuffer *MutableBuffer::appendRexx(RexxObject **args, size_t argc)
{
    // if the count is zero, this is an error.  process as if we had
    // gotten a null first argument
    if (argc == 0)
    {
        stringArgument(OREF_NULL, ARG_ONE);
    }

    // append each of the arguments
    for (size_t i = 0; i < argc; i++)
    {
        Protected<RexxString> string = stringArgument(args[i], i + 1);
        // make sure we have enough room
        ensureCapacity(string->getLength());

        copyData(dataLength, string->getStringData(), string->getLength());
        dataLength += string->getLength();
    }

    return this;
}


/**
 * Rexx method for setting a mutable buffer to a given
 * text value.
 *
 * @param obj    The argument object.
 *
 * @return Always returns the mutablebuffer instance.
 */
MutableBuffer *MutableBuffer::setTextRexx(RexxObject *obj)
{
    Protected<RexxString> string = stringArgument(obj, ARG_ONE);
    setText(string);
    return this;
}


/**
 * Primitive level method for setting a mutable buffer to
 * a given text value.
 *
 * @param string The new string value for the buffer.
 */
void MutableBuffer::setText(RexxString *string)
{
    // set the data length to zero
    dataLength = 0;
    // and append the new text to the buffer
    append(string->getStringData(), string->getLength());
}


/**
 * Append a value to this buffer.
 *
 * @param d      The data to append.
 * @param l      The length of the data to append.
 */
void MutableBuffer::append(const char *d, size_t l)
{
    // make sure we have enough room
    ensureCapacity(l);

    copyData(dataLength, d, l);
    dataLength += l;
}


/**
 * Append a character to this buffer.
 *
 * @param c      The character to append.
 */
void MutableBuffer::append(char c)
{
    // make sure we have enough room
    ensureCapacity(1);

    copyData(dataLength, &c, 1);
    dataLength += 1;
}



/**
 * insert string at given position
 *
 * @param str    The string to insert.
 * @param pos    The starting position.
 * @param len    The insertion length.
 * @param pad    Any pad character.
 *
 * @return The same mutable buffer object.
 */
MutableBuffer *MutableBuffer::insert(RexxObject *str, RexxObject *pos, RexxObject *len, RexxObject *pad)
{
    // force this into string form
    RexxString * string = stringArgument(str, ARG_ONE);

    // we're using optional length because 0 is valid for insert.
    size_t begin = optionalNonNegative(pos, 0, ARG_TWO);
    size_t insertLength = optionalLengthArgument(len, string->getLength(), ARG_THREE);

    char padChar = optionalPadArgument(pad, ' ', ARG_FOUR);

    size_t copyLength = std::min(insertLength, string->getLength());
    size_t padLength = insertLength - copyLength;


    // if inserting within the current bounds, we only need to add the length
    // if inserting beyond the end, we need to make sure we add space for the gap too
    if (begin < dataLength)
    {
        // if inserting a zero length string, this is simple!
        if (insertLength == 0)
        {
            return this;
        }
        ensureCapacity(insertLength);
    }
    else
    {
        ensureCapacity(insertLength + (begin - dataLength));
    }


    // create space in the buffer
    if (begin < dataLength)
    {
        openGap(begin, insertLength, dataLength - begin);
    }
    // else if we need to pad before the insertion
    else if (begin > dataLength)
    {
        setData(dataLength, padChar, begin - dataLength);
    }
    // copy in the string contents
    copyData(begin, string->getStringData(), copyLength);
    // do we need data padding?
    if (padLength > 0)
    {
        setData(begin + string->getLength(), padChar, padLength);
    }
    // inserting after the end? the resulting length is measured from the insertion point
    if (begin > dataLength)
    {
        dataLength = begin + insertLength;
    }
    else
    {
        // just add in the inserted length
        dataLength += insertLength;
    }
    return this;
}


/**
 * Overlay a string value on the buffer data.
 *
 * @param str    The string to overlay.
 * @param pos    The position to start the overlay.
 * @param len    The length of the overlay.
 * @param pad    An optional pad character.
 *
 * @return The same mutable buffer.
 */
MutableBuffer *MutableBuffer::overlay(RexxObject *str, RexxObject *pos, RexxObject *len, RexxObject *pad)
{
    RexxString *string = stringArgument(str, ARG_ONE);
    size_t begin = optionalPositionArgument(pos, 1, ARG_TWO) - 1;
    size_t replaceLength = optionalLengthArgument(len, string->getLength(), ARG_THREE);

    char padChar = optionalPadArgument(pad, ' ', ARG_FOUR);

    // make sure we have room for this
    ensureCapacity(begin + replaceLength);

    // is our start position beyond the current data end?
    if (begin > dataLength)
    {
        // add padding to the gap
        setData(dataLength, padChar, begin - dataLength);
    }

    // now overlay the string data
    copyData(begin, string->getStringData(), std::min(replaceLength, string->getLength()));
    // do we need additional padding?
    if (replaceLength > string->getLength())
    {
        // pad the section after the overlay
        setData(begin + string->getLength(), padChar, replaceLength - string->getLength());
    }

    // did this add to the size?
    if (begin + replaceLength > dataLength)
    {
        //adjust upward
        dataLength = begin + replaceLength;
    }
    return this;
}


/**
 * Replace a target substring within a string with
 * a new string value.  This is similar overlay, but
 * replacing might cause the characters following the
 * replacement position to be shifted to the left or
 * right.  This is the "[]=" form.
 *
 * @param str    The replacement string.
 * @param pos    The target position (required).
 * @param len    The target length (optional).  If not specified, the
 *               length of the replacement string is used, and this
 *               is essentially an overlay operation.
 *
 * @return The target mutablebuffer object.
 */
MutableBuffer *MutableBuffer::bracketsEqual(RexxObject *str, RexxObject *pos, RexxObject *len)
{
    // this is just replaceAt with the default pad.
    return replaceAt(str, pos, len, OREF_NULL);
}


/**
 * Replace a target substring within a string with
 * a new string value.  This is similar overlay, but
 * replacing might cause the characters following the
 * replacement position to be shifted to the left or
 * right.
 *
 * @param str    The replacement string.
 * @param pos    The target position (required).
 * @param len    The target length (optional).  If not specified, the
 *               length of the replacement string is used, and this
 *               is essentially an overlay operation.
 * @param pad    A padding character if padding is required.  The default
 *               pad is a ' '.  Padding only occurs if the replacement
 *               position is beyond the current data length.
 *
 * @return The target mutablebuffer object.
 */
MutableBuffer *MutableBuffer::replaceAt(RexxObject *str, RexxObject *pos, RexxObject *len, RexxObject *pad)
{
    RexxString *string = stringArgument(str, "new");
    size_t begin = positionArgument(pos, "position") - 1;
    size_t newLength = string->getLength();
    size_t replaceLength = optionalLengthArgument(len, newLength, "length");

    char padChar = optionalPadArgument(pad, ' ', "pad");
    size_t finalLength;

    // if replaceLength extends beyond the end of the string
    //    then we cut it.
    if (begin > dataLength)
    {
       replaceLength = 0;
    }
    else if (begin + replaceLength > dataLength)
    {
       replaceLength = dataLength - begin;
    }

    // We need to add the delta between the excised string and the inserted
    // replacement string.
    //
    // If this extends beyond the end of the string, then we require space for
    // the position + the replacement string length.  Else we find the required
    // size (may be smaller than before)
    if (begin > dataLength)
    {
        finalLength = begin - replaceLength + newLength;
    }
    else
    {
        finalLength = dataLength - replaceLength + newLength;
    }

    // make sure we have room for this
    ensureCapacity(finalLength);

    // is our start position beyond the current data end?
    // NB: Even though we've adjusted the buffer size, the dataLength is still
    // the original entry length.
    if (begin > dataLength)
    {
        // add padding to the gap
        setData(dataLength, padChar, begin - dataLength);
        // now overlay the string data
        copyData(begin, string->getStringData(), newLength);
    }
    else
    {
        // if the strings are of different lengths, we need to adjust the size
        // of the gap we're copying into.  Only adjust if there is a real gap
        if (replaceLength != newLength && begin + replaceLength < dataLength)
        {
            // snip out the original string
            adjustGap(begin, replaceLength, newLength);
        }
        // now overlay the string data
        copyData(begin, string->getStringData(), newLength);
    }

    // and finally adjust the length
    dataLength = finalLength;
    // our return value is always the target mutable buffer
    return this;
}


/**
 * Delete a character range in the buffer.
 *
 * @param _start The starting delete position.
 * @param len    The length to delete.
 *
 * @return The same buffer.
 */
MutableBuffer *MutableBuffer::mydelete(RexxObject *_start, RexxObject *len)
{
    size_t begin = optionalPositionArgument(_start, 1, ARG_ONE) - 1;
    size_t range = optionalLengthArgument(len, data->getDataLength() - begin, ARG_TWO);

    // is the begin point actually within the string?
    if (begin < dataLength)
    {
        // deleting from the middle?
        if (begin + range < dataLength)
        {
            // shift everything over
            closeGap(begin, range, dataLength - (begin + range));
            dataLength -= range;
        }
        else
        {
            // we're just truncating
            dataLength = begin;
        }
    }
    return this;
}


/**
 * Set the size of the buffer.
 *
 * @param size   The size to set.
 *
 * @return Returns the same mutable buffer.
 */
RexxObject *MutableBuffer::setBufferSize(RexxInteger *size)
{
    size_t newsize = lengthArgument(size, ARG_ONE);
    // has a reset to zero been requested?
    if (newsize == 0)
    {
        // have we increased the buffer size?
        if (bufferLength > defaultSize)
        {
            // reallocate the buffer
            OrefSet(this, data, new_buffer(defaultSize));
            // reset the size to the default
            bufferLength = defaultSize;
        }
        dataLength = 0;
    }
    // an actual resize?
    else if (newsize != bufferLength)
    {
        // reallocate the buffer
        BufferClass *newBuffer = new_buffer(newsize);
        // if we're shrinking this, it truncates.
        dataLength = std::min(dataLength, newsize);
        newBuffer->copyData(0, data->getData(), dataLength);
        // replace the old buffer
        setField(data, newBuffer);
        // and update the size....
        bufferLength = newsize;
    }
    return this;
}


/**
 * Handle a REQUEST('STRING') request for a mutablebuffer object
 *
 * @return the string value of the buffer.
 */
RexxString *MutableBuffer::makeString()
{
    return new_string(data->getData(), dataLength);
}


/**
 * Baseclass optimization for handling request array calls.
 *
 * @return The string object converted to an array using default arguments.
 */
ArrayClass  *MutableBuffer::makeArray()
{
    // forward to the Rexx version with default arguments
    return makeArrayRexx(OREF_NULL);
}


/**
 * Return the primitive string value of this object
 *
 * @return this always just forwards to makeString
 */
RexxString* MutableBuffer::stringValue()
{
    return makeString();
}


/**
 * Handle the primitive class makeString optimization.  This
 * is required because MutableBuffer implements a
 * STRING method.
 *
 * @return The string value of the buffer
 */
RexxString *MutableBuffer::primitiveMakeString()
{
    // go straight to the string handler
    return makeString();
}


/**
 * Extract a substring from the the buffer data.
 *
 * @param argposition
 *                  The starting position of the substring.
 * @param arglength The length to extract.
 * @param pad       An optional pad character.
 *
 * @return The extraced substring, as a string.
 */
RexxString *MutableBuffer::substr(RexxInteger *argposition, RexxInteger *arglength, RexxString  *pad)
{
    return StringUtil::substr(getStringData(), getLength(), argposition, arglength, pad);
}


/**
 * Extract a substring from the the buffer data, using brackets
 * semantics.
 *
 * @param argposition
 *                  The starting position of the substring.
 * @param arglength The length to extract.
 * @param pad       An optional pad character.
 *
 * @return The extraced substring, as a string.
 */
RexxString *MutableBuffer::brackets(RexxInteger *argposition, RexxInteger *arglength)
{
    return StringUtil::substr(getStringData(), getLength(), argposition, arglength);
}


/**
 * Perform a search for a string within the buffer.
 *
 * @param needle The search needle.
 * @param pstart the starting position.
 * @param range  The length of the search range.
 *
 * @return The index of the located string.  Returns 0 if no matches
 *         are found.
 */
RexxInteger *MutableBuffer::posRexx(RexxString  *needle, RexxInteger *pstart, RexxInteger *range)
{
    return StringUtil::posRexx(getStringData(), getLength(), needle, pstart, range);
}


/**
 * Test if the buffer contains a given string within a specified
 * range.
 *
 * @param needle The search needle.
 * @param pstart the starting position.
 * @param range  The length of the search range.
 *
 * @return .true if the string is found, .false otherwise
 */
RexxObject *MutableBuffer::containsRexx(RexxString  *needle, RexxInteger *pstart, RexxInteger *range)
{
    return StringUtil::containsRexx(getStringData(), getLength(), needle, pstart, range);
}


/**
 * Perform a search for the last position of a string within the
 * buffer.
 *
 * @param needle The search needle.
 * @param pstart the starting position.
 *
 * @return The index of the located string.  Returns 0 if no matches
 *         are found.
 */
RexxInteger *MutableBuffer::lastPos(RexxString  *needle, RexxInteger *_start, RexxInteger *_range)
{
    return StringUtil::lastPosRexx(getStringData(), getLength(), needle, _start, _range);
}


/**
 * Perform a caseless search for a string within the buffer.
 *
 * @param needle The search needle.
 * @param pstart the starting position.
 * @param range  The length of the range to search in.
 *
 * @return The index of the located string.  Returns 0 if no matches
 *         are found.
 */
RexxInteger *MutableBuffer::caselessPos(RexxString  *needle, RexxInteger *pstart, RexxInteger *range)
{
    needle = stringArgument(needle, ARG_ONE);
    size_t _start = optionalPositionArgument(pstart, 1, ARG_TWO);
    size_t _range = optionalLengthArgument(range, getLength() - _start + 1, ARG_THREE);

    return new_integer(StringUtil::caselessPos(getStringData(), getLength(), needle , _start - 1, _range));
}

/**
 * Perform a caseless search for a string within the buffer.
 *
 * @param needle The search needle.
 * @param pstart the starting position.
 * @param range  The length of the range to search in.
 *
 * @return .true if the string is found, .false otherwise.
 */
RexxObject *MutableBuffer::caselessContains(RexxString  *needle, RexxInteger *pstart, RexxInteger *range)
{
    needle = stringArgument(needle, ARG_ONE);
    size_t _start = optionalPositionArgument(pstart, 1, ARG_TWO);
    size_t _range = optionalLengthArgument(range, getLength() - _start + 1, ARG_THREE);

    // pass on to the primitive function and return as an integer object
    return booleanObject(StringUtil::caselessPos(getStringData(), getLength(), needle , _start - 1, _range) > 0);
}


/**
 * Perform a caseless search for the last position of a string
 * within the buffer.
 *
 * @param needle The search needle.
 * @param pstart the starting position.
 *
 * @return The index of the located string.  Returns 0 if no matches
 *         are found.
 */
RexxInteger *MutableBuffer::caselessLastPos(RexxString  *needle, RexxInteger *pstart, RexxInteger *range)
{
    needle = stringArgument(needle, ARG_ONE);
    size_t _start = optionalPositionArgument(pstart, getLength(), ARG_TWO);
    size_t _range = optionalLengthArgument(range, getLength(), ARG_THREE);

    // pass on to the primitive function and return as an integer object
    return new_integer(StringUtil::caselessLastPos(getStringData(), getLength(), needle , _start, _range));
}


/**
 * Extract a single character from a string object.
 * Returns a null string if the specified position is
 * beyond the bounds of the string.
 *
 * @param positionArg
 *               The position of the target  character.  Must be a positive
 *               whole number.
 *
 * @return Returns the single character at the target position.
 *         Returns a null string if the position is beyond the end
 *         of the string.
 */
RexxString *MutableBuffer::subchar(RexxInteger *positionArg)
{
    return StringUtil::subchar(getStringData(), getLength(), positionArg);
}


/**
 * Split buffer data into an array into an array
 *
 * @param div    The optional separator string.
 *
 * @return An array of the split data.
 */
ArrayClass *MutableBuffer::makeArrayRexx(RexxString *div)
{
    return StringUtil::makearray(getStringData(), getLength(), div);
}


/**
 * Count occurrences of one string in another.
 *
 * @param needle The needle string to count.
 *
 * @return The count of occurrences of the string in the buffer.
 */
RexxInteger *MutableBuffer::countStrRexx(RexxString *needle)
{
    needle = stringArgument(needle, ARG_ONE);
    // delegate the counting to the string util
    return new_integer(StringUtil::countStr(getStringData(), getLength(), needle, Numerics::MAX_WHOLENUMBER));
}


/**
 * Count occurrences of one string in another, using caseless comparisons.
 *
 * @param needle The target search needle.
 *
 * @return The count of the characters.
 */
RexxInteger *MutableBuffer::caselessCountStrRexx(RexxString *needle)
{
    needle = stringArgument(needle, ARG_ONE);
    // delegate the counting to the string util
    return new_integer(StringUtil::caselessCountStr(getStringData(), getLength(), needle, Numerics::MAX_WHOLENUMBER));
}

/**
 * Do an inplace changeStr operation on a mutablebuffer.
 *
 * @param needle    The search needle.
 * @param newNeedle The replacement string.
 * @param countArg  The number of occurrences to replace.
 *
 * @return The target MutableBuffer
 */
MutableBuffer *MutableBuffer::changeStr(RexxString *needle, RexxString *newNeedle, RexxInteger *countArg)
{
    needle = stringArgument(needle, ARG_ONE);
    newNeedle = stringArgument(newNeedle, ARG_TWO);

    // we'll only change up to a specified count.  If not there, we do everything.
    size_t count = optionalNonNegative(countArg, Numerics::MAX_WHOLENUMBER, ARG_THREE);

    // if no change is requested, return the original string.
    if (count == 0)
    {
        return this;
    }

    size_t needleLength = needle->getLength();
    // if needle is the nullstring, return the original string.
    if (needleLength == 0)
    {
        return this;
    }
    size_t newLength = newNeedle->getLength();
    const char *newData = newNeedle->getStringData();

    // an inplace update has complications, depending on whether the new string is shorter,
    // the same length, or longer

    size_t matches;

    // simplest case...same length strings.  We can just overlay the existing occurrences
    if (needleLength == newLength)
    {
        const char *source = getStringData();
        size_t sourceLength = getLength();
        size_t _start = 0;                          /* set a zero starting point         */
        // search for at most 'count' occurences
        for (matches = 1; matches <= count; matches++)
        {
            // search for the next occurrence
            size_t matchPos = StringUtil::pos(source, sourceLength, needle, _start, sourceLength);
            if (matchPos == 0)         // no more match? we're done
            {
                break;
            }
            copyData(matchPos - 1, newData, newLength);
            // step to the next search position
            _start = matchPos + newLength - 1;
        }
        // no need to update the result length as it will be the same
    }

    // this will be a shorter thing, so we can do things in place as if we were using two buffers
    else if (needleLength > newLength)
    {
        // we start building from the beginning
        size_t copyOffset = 0;
        size_t _start = 0;
        // get our string bounds
        const char *source = getStringData();
        size_t sourceLength = getLength();
        // search for at most 'count' occurences
        for (matches = 0; matches < count; matches++)
        {
            // search for the next occurrence
            size_t matchPos = StringUtil::pos(source, sourceLength, needle, _start, sourceLength);
            if (matchPos == 0)         // no more match? we're done
            {
                break;
            }

            // replace match
            size_t copyLength = (matchPos - 1) - _start;  /* get the next length to copy       */
            // if this skipped over characters, we need to copy those
            if (copyLength != 0)
            {
                copyData(copyOffset, source + _start, copyLength);
                copyOffset += copyLength;
            }
            // replacing with a non-null string, copy the replacement string in
            if (newLength != 0)
            {
                copyData(copyOffset, newData, newLength);
                copyOffset += newLength;
            }
            _start = matchPos + needleLength - 1;  /* step to the next position         */
        }
        // we likely have some remainder that needs copying
        if (_start < sourceLength)
        {
            copyData(copyOffset, source + _start, sourceLength - _start);
        }

        // update the result length
        dataLength = sourceLength - matches * (needleLength - newLength);
    }

    // hardest case...the string gets longer.  We need to shift all of the data
    // to the end and then pull the pieces back in as we go
    else
    {
        // find the number of matches (up to count) in the string
        size_t matches = StringUtil::countStr(getStringData(), getLength(), needle, count);

        // no matches is easy!
        if (matches == 0)
        {
            return this;
        }

        // calculate the final length and make sure we have enough space
        size_t resultLength = getLength() + matches * (newLength - needleLength);
        ensureCapacity(resultLength);

        size_t growth = (newLength - needleLength) * matches;

        // we start building from the beginning
        size_t copyOffset = 0;
        size_t _start = 0;
        // get our string bounds
        const char *source = getStringData() + growth;
        size_t sourceLength = getLength();
        // this shifts everything to the end of the buffer.  From there,
        // we pull pieces back into place.
        openGap(0, growth, sourceLength);
        // this is our scan offset
        for (size_t i = 0; i < matches; i++)
        {
            // look for each instance and replace
            size_t matchPos = StringUtil::pos(source, sourceLength, needle, _start, sourceLength);
            size_t copyLength = (matchPos - 1) - _start;  /* get the next length to copy       */
            // if this skipped over characters, we need to copy those
            if (copyLength != 0)
            {
                copyData(copyOffset, source + _start, copyLength);
                copyOffset += copyLength;
            }
            // replacing with a non-null string, copy the replacement string in
            if (newLength != 0)
            {
                copyData(copyOffset, newData, newLength);
                copyOffset += newLength;
            }
            _start = matchPos + needleLength - 1;  /* step to the next position         */
        }
        // we likely have some remainder that needs copying
        if (_start < sourceLength)
        {
            copyData(copyOffset, source + _start, sourceLength - _start);
        }
        // update the result length for the longer result
        dataLength = resultLength;
    }
    return this;
}


/**
 * Do an inplace caseless changeStr operation on a
 * mutablebuffer.
 *
 * @param needle    The search needle.
 * @param newNeedle The replacement string.
 * @param countArg  The number of occurrences to replace.
 *
 * @return The target MutableBuffer
 */
MutableBuffer *MutableBuffer::caselessChangeStr(RexxString *needle, RexxString *newNeedle, RexxInteger *countArg)
{
    needle = stringArgument(needle, ARG_ONE);
    newNeedle = stringArgument(newNeedle, ARG_TWO);

    // we'll only change up to a specified count.  If not there, we do everything.
    size_t count = optionalNonNegative(countArg, Numerics::MAX_WHOLENUMBER, ARG_THREE);

    // if no change is requested, return the original string.
    if (count == 0)
    {
        return this;
    }

    size_t needleLength = needle->getLength();
    // if needle is the nullstring, return the original string.
    if (needleLength == 0)
    {
        return this;
    }
    size_t newLength = newNeedle->getLength();
    const char *newData = newNeedle->getStringData();

    // an inplace update has complications, depending on whether the new string is shorter,
    // the same length, or longer

    size_t matches;

    // simplest case...same length strings.  We can just overlay the existing occurrences
    if (needleLength == newLength)
    {
        const char *source = getStringData();
        size_t sourceLength = getLength();
        size_t _start = 0;                          /* set a zero starting point         */
        // search for at most 'count' occurences
        for (matches = 1; matches <= count; matches++)
        {
            // search for the next occurrence
            size_t matchPos = StringUtil::caselessPos(source, sourceLength, needle, _start, sourceLength);
            if (matchPos == 0)         // no more match? we're done
            {
                break;
            }
            copyData(matchPos - 1, newData, newLength);
            // step to the next search position
            _start = matchPos + newLength - 1;
        }
        // no need to update the result length as it will be the same
    }

    // this will be a shorter thing, so we can do things in place as if we were using two buffers
    else if (needleLength > newLength)
    {
        // we start building from the beginning
        size_t copyOffset = 0;
        size_t _start = 0;
        // get our string bounds
        const char *source = getStringData();
        size_t sourceLength = getLength();
        // search for at most 'count' occurences
        for (matches = 0; matches < count; matches++)
        {
            // search for the next occurrence
            size_t matchPos = StringUtil::caselessPos(source, sourceLength, needle, _start, sourceLength);
            if (matchPos == 0)         // no more match? we're done
            {
                break;
            }

            // replace match
            size_t copyLength = (matchPos - 1) - _start;  /* get the next length to copy       */
            // if this skipped over characters, we need to copy those
            if (copyLength != 0)
            {
                copyData(copyOffset, source + _start, copyLength);
                copyOffset += copyLength;
            }
            // replacing with a non-null string, copy the replacement string in
            if (newLength != 0)
            {
                copyData(copyOffset, newData, newLength);
                copyOffset += newLength;
            }
            _start = matchPos + needleLength - 1;  /* step to the next position         */
        }
        // we likely have some remainder that needs copying
        if (_start < sourceLength)
        {
            copyData(copyOffset, source + _start, sourceLength - _start);
        }

        // update the result length
        dataLength = sourceLength - matches * (needleLength - newLength);
    }

    // hardest case...the string gets longer.  We need to shift all of the data
    // to the end and then pull the pieces back in as we go
    else
    {
        // find the number of matches (up to count) in the string
        size_t matches = StringUtil::caselessCountStr(getStringData(), getLength(), needle, count);

        // no matches is easy!
        if (matches == 0)
        {
            return this;
        }

        // calculate the final length and make sure we have enough space
        size_t resultLength = getLength() + matches * (newLength - needleLength);
        ensureCapacity(resultLength);

        size_t growth = (newLength - needleLength) * matches;

        // we start building from the beginning
        size_t copyOffset = 0;
        size_t _start = 0;
        // get our string bounds
        const char *source = getStringData() + growth;
        size_t sourceLength = getLength();
        // this shifts everything to the end of the buffer.  From there,
        // we pull pieces back into place.
        openGap(0, growth, sourceLength);
        // this is our scan offset
        for (size_t i = 0; i < matches; i++)
        {
            // look for each instance and replace
            size_t matchPos = StringUtil::caselessPos(source, sourceLength, needle, _start, sourceLength);
            size_t copyLength = (matchPos - 1) - _start;  /* get the next length to copy       */
            // if this skipped over characters, we need to copy those
            if (copyLength != 0)
            {
                copyData(copyOffset, source + _start, copyLength);
                copyOffset += copyLength;
            }
            // replacing with a non-null string, copy the replacement string in
            if (newLength != 0)
            {
                copyData(copyOffset, newData, newLength);
                copyOffset += newLength;
            }
            _start = matchPos + needleLength - 1;  /* step to the next position         */
        }
        // we likely have some remainder that needs copying
        if (_start < sourceLength)
        {
            copyData(copyOffset, source + _start, sourceLength - _start);
        }
        // update the result length for the longer result
        dataLength = resultLength;
    }
    return this;
}


/**
 * Rexx exported method stub for the lower() method.
 *
 * @param start  The optional starting location.  Defaults to the first character
 *               if not specified.
 * @param length The length to convert.  Defaults to the segment from the start
 *               position to the end of the string.
 *
 * @return A new string object with the case conversion applied.
 */
MutableBuffer *MutableBuffer::lower(RexxInteger *_start, RexxInteger *_length)
{
    size_t startPos = optionalPositionArgument(_start, 1, ARG_ONE) - 1;
    size_t rangeLength = optionalLengthArgument(_length, getLength(), ARG_TWO);

    // if we're starting beyond the end bounds, return unchanged
    if (startPos >= getLength())
    {
        return this;
    }

    rangeLength = std::min(rangeLength, getLength() - startPos);

    // a zero length value is also a non-change.
    if (rangeLength == 0)
    {
        return this;
    }

    char *bufferData = getData() + startPos;
    // now uppercase in place
    for (size_t i = 0; i < rangeLength; i++)
    {
        *bufferData = tolower(*bufferData);
        bufferData++;
    }
    return this;
}


/**
 * Rexx exported method stub for the upper() method.
 *
 * @param start  The optional starting location.  Defaults to the first character
 *               if not specified.
 * @param length The length to convert.  Defaults to the segment from the start
 *               position to the end of the string.
 *
 * @return A new string object with the case conversion applied.
 */
MutableBuffer *MutableBuffer::upper(RexxInteger *_start, RexxInteger *_length)
{
    size_t startPos = optionalPositionArgument(_start, 1, ARG_ONE) - 1;
    size_t rangeLength = optionalLengthArgument(_length, getLength(), ARG_TWO);

    // if we're starting beyond the end bounds, return unchanged
    if (startPos >= getLength())
    {
        return this;
    }

    rangeLength = std::min(rangeLength, getLength() - startPos);

    // a zero length value is also a non-change.
    if (rangeLength == 0)
    {
        return this;
    }

    char *bufferData = getData() + startPos;
    // now uppercase in place
    for (size_t i = 0; i < rangeLength; i++)
    {
        *bufferData = toupper(*bufferData);
        bufferData++;
    }
    return this;
}


/**
 * translate characters in the buffer using a translation table.
 *
 * @param tableo The output table specification
 * @param tablei The input table specification
 * @param pad    An optional padding character (default is a space).
 * @param _start The starting position to translate.
 * @param _range The length to translate
 *
 * @return The target mutable buffer.
 */
MutableBuffer *MutableBuffer::translate(RexxString *tableo, RexxString *tablei, RexxString *pad, RexxInteger *_start, RexxInteger *_range)
{
    // just a simple uppercase?
    if (tableo == OREF_NULL && tablei == OREF_NULL && pad == OREF_NULL)
    {
        return upper(_start, _range);
    }

    tableo = optionalStringArgument(tableo, GlobalNames::NULLSTRING, ARG_ONE);
    size_t outTableLength = tableo->getLength();

    tablei = optionalStringArgument(tablei, GlobalNames::NULLSTRING, ARG_TWO);
    size_t inTableLength = tablei->getLength();
    const char *inTable = tablei->getStringData();
    const char *outTable = tableo->getStringData();

    char padChar = optionalPadArgument(pad, ' ', ARG_THREE);
    size_t startPos = optionalPositionArgument(_start, 1, ARG_FOUR);
    size_t range = optionalLengthArgument(_range, getLength() - startPos + 1, ARG_FOUR);

    // if nothing to translate, we can return now
    if (startPos > getLength() || range == 0)
    {
        return this;
    }

    // capy the real range
    range = std::min(range, getLength() - startPos + 1);
    char *scanPtr = getData() + startPos - 1;
    size_t scanLength = range;

    while (scanLength--)
    {
        char ch = *scanPtr;
        size_t position;

        if (tablei->getLength() != 0)
        {
            position = StringUtil::memPos(inTable, inTableLength, ch);
        }
        else
        {
            position = ((size_t)ch) & 0xff;
        }
        // if found in the table, need to convert
        if (position != (size_t)(-1))
        {
            if (position < outTableLength)
            {
                *scanPtr = *(outTable + position);
            }
            // use the pad character if the output table is shorter
            else
            {
                *scanPtr = padChar;
            }
        }
        scanPtr++;
    }
    return this;
}


/**
 * Test if regions within two strings match.
 *
 * @param start_  The starting compare position within the target string.  This
 *                must be within the bounds of the string.
 * @param other   The other compare string.
 * @param offset_ The starting offset of the compare string.  This must be
 *                within the string bounds.  The default start postion is 1.
 * @param len_    The length of the compare substring.  The length and the
 *                offset must specify a valid substring of other.  If not
 *                specified, this defaults to the substring from the
 *                offset to the end of the string.
 *
 * @return True if the two regions match, false for any mismatch.
 */
RexxObject *MutableBuffer::match(RexxInteger *start_, RexxString *other, RexxInteger *offset_, RexxInteger *len_)
{
    size_t _start = positionArgument(start_, ARG_ONE);
    // the start position must be within the string bounds for any match
    // to be performed.
    if (_start > getLength())
    {
        return TheFalseObject;
    }
    other = stringArgument(other, ARG_TWO);

    size_t offset = optionalPositionArgument(offset_, 1, ARG_THREE);

    // other offset/positional problems are also always a false result
    if (offset_ != OREF_NULL && offset > other->getLength())
    {
        return TheFalseObject;
    }

    size_t len = optionalLengthArgument(len_, other->getLength() - offset + 1, ARG_FOUR);

    if ((offset + len - 1) > other->getLength())
    {
        return TheFalseObject;
    }

    return booleanObject(primitiveMatch(_start, other, offset, len));
}


/**
 * Test if regions within two strings match.
 *
 * @param start_  The starting compare position within the target string.  This
 *                must be within the bounds of the string.
 * @param other   The other compare string.
 * @param offset_ The starting offset of the compare string.  This must be
 *                within the string bounds.  The default start postion is 1.
 * @param len_    The length of the compare substring.  The length and the
 *                offset must specify a valid substring of other.  If not
 *                specified, this defaults to the substring from the
 *                offset to the end of the string.
 *
 * @return True if the two regions match, false for any mismatch.
 */
RexxObject *MutableBuffer::caselessMatch(RexxInteger *start_, RexxString *other, RexxInteger *offset_, RexxInteger *len_)
{
    size_t _start = positionArgument(start_, ARG_ONE);
    // the start position must be within the string bounds for any match
    // to be performed.
    if (_start > getLength())
    {
        return TheFalseObject;
    }
    other = stringArgument(other, ARG_TWO);

    size_t offset = optionalPositionArgument(offset_, 1, ARG_THREE);

    if (offset_ != OREF_NULL && offset > other->getLength())
    {
        return TheFalseObject;
    }

    size_t len = optionalLengthArgument(len_, other->getLength() - offset + 1, ARG_FOUR);

    if ((offset + len - 1) > other->getLength())
    {
        return TheFalseObject;
    }

    return booleanObject(primitiveCaselessMatch(_start, other, offset, len));
}


/**
 * Test if a string starts with another string
 *
 * @param other  The other compare string.
 *
 * @return True if the string starts with the other, false otherwise.
 */
RexxObject *MutableBuffer::startsWithRexx(RexxString *other)
{
    other = stringArgument(other, "match");
    return booleanObject(primitiveMatch(1, other, 1, other->getLength()));
}


/**
 * Test if a string starts with another string
 *
 * @param other  The other compare string.
 *
 * @return True if the string starts with the other, false otherwise.
 */
RexxObject *MutableBuffer::caselessStartsWithRexx(RexxString *other)
{
    other = stringArgument(other, "match");
    return booleanObject(primitiveCaselessMatch(1, other, 1, other->getLength()));
}


/**
 * Test if a string ends with another string
 *
 * @param other  The other compare string.
 *
 * @return True if the string starts with the other, false otherwise.
 */
RexxObject *MutableBuffer::endsWithRexx(RexxString *other)
{
    other = stringArgument(other, "match");

    // we need to check this here, because the calculated offset might be wrong
    if (other->getLength() > getLength())
    {
        return TheFalseObject;
    }

    return booleanObject(primitiveMatch(getLength() - other->getLength() + 1, other, 1, other->getLength()));
}


/**
 * Test if a string ends with another string
 *
 * @param other  The other compare string.
 *
 * @return True if the string starts with the other, false otherwise.
 */
RexxObject *MutableBuffer::caselessEndsWithRexx(RexxString *other)
{
    other = stringArgument(other, "match");

    // we need to check this here, because the calculated offset might be wrong
    if (other->getLength() > getLength())
    {
        return TheFalseObject;
    }

    return booleanObject(primitiveCaselessMatch(getLength() - other->getLength() + 1, other, 1, other->getLength()));
}


/**
 * Perform a compare of regions of two string objects.  Returns
 * true if the two regions match, returns false for mismatches.
 *
 * @param start  The starting offset within the target string.
 * @param other  The source string for the compare.
 * @param offset The offset of the substring of the other string to use.
 * @param len    The length of the substring to compare.
 *
 * @return True if the regions match, false otherwise.
 */
bool MutableBuffer::primitiveMatch(size_t _start, RexxString *other, size_t offset, size_t len)
{
    _start--;      // make the starting point origin zero
    offset--;

    // if the match is not possible in the target string, just return false now.
    if ((_start + len) > getLength() || len == 0)
    {
        return false;
    }

    return memcmp(getStringData() + _start, other->getStringData() + offset, len) == 0;
}


/**
 * Perform a caselesee compare of regions of two string objects.
 * Returns true if the two regions match, returns false for
 * mismatches.
 *
 * @param start  The starting offset within the target string.
 * @param other  The source string for the compare.
 * @param offset The offset of the substring of the other string to use.
 * @param len    The length of the substring to compare.
 *
 * @return True if the regions match, false otherwise.
 */
bool MutableBuffer::primitiveCaselessMatch(size_t _start, RexxString *other, size_t offset, size_t len)
{
    _start--;      // make the starting point origin zero
    offset--;

    // if the match is not possible in the target string, just return false now.
    if ((_start + len) > getLength() || len == 0)
    {
        return false;
    }

    return StringUtil::caselessCompare(getStringData() + _start, other->getStringData() + offset, len) == 0;
}


/**
 * Compare a single character at a give position against
 * a set of characters to see if any of the characters is
 * a match.
 *
 * @param position_ The character position
 * @param matchSet  The set to compare against.
 *
 * @return true if the character at the give position is any of the characters,
 *         false if none of them match.
 */
RexxObject *MutableBuffer::matchChar(RexxInteger *position_, RexxString *matchSet)
{
    size_t position = positionArgument(position_, ARG_ONE);
    // the start position must be within the string bounds for any match
    // to be performed.
    if (position > getLength())
    {
        return TheFalseObject;
    }
    matchSet = stringArgument(matchSet, ARG_TWO);

    size_t _setLength = matchSet->getLength();
    char         _matchChar = getChar(position - 1);

    // iterate through the match set looking for a match
    for (size_t i = 0; i < _setLength; i++)
    {
        if (_matchChar == matchSet->getChar(i))
        {
            return TheTrueObject;
        }
    }
    return TheFalseObject;
}


/**
 * Compare a single character at a give position against
 * a set of characters to see if any of the characters is
 * a match.
 *
 * @param position_ The character position
 * @param matchSet  The set to compare against.
 *
 * @return true if the character at the give position is any of the characters,
 *         false if none of them match.
 */
RexxObject *MutableBuffer::caselessMatchChar(RexxInteger *position_, RexxString *matchSet)
{
    size_t position = positionArgument(position_, ARG_ONE);
    // the start position must be within the string bounds
    // the start position must be within the string bounds for any match
    // to be performed.
    if (position > getLength())
    {
        return TheFalseObject;
    }
    matchSet = stringArgument(matchSet, ARG_TWO);

    size_t _setLength = matchSet->getLength();
    char         _matchChar = getChar(position - 1);
    _matchChar = toupper(_matchChar);

    // iterate through the match set looking for a match, using a
    // caseless compare
    for (size_t i = 0; i < _setLength; i++)
    {
        if (_matchChar == toupper(matchSet->getChar(i)))
        {
            return TheTrueObject;
        }
    }
    return TheFalseObject;
}


/**
 * Perform a character verify operation on a mutable buffer.
 *
 * @param ref    The reference string.
 * @param option The match/nomatch option.
 * @param _start The start position for the verify.
 * @param range  The range to search
 *
 * @return The offset of the first match/mismatch within the buffer.
 */
RexxInteger *MutableBuffer::verify(RexxString *ref, RexxString *option, RexxInteger *_start, RexxInteger *range)
{
    return StringUtil::verify(getStringData(), getLength(), ref, option, _start, range);
}


/**
 * Perform a subword extraction from a mutable buffer.
 *
 * @param position The first word to be extracted.
 * @param plength  The number of words to extract.
 *
 * @return The substring containing the extacted words.
 */
RexxString *MutableBuffer::subWord(RexxInteger *position, RexxInteger *plength)
{
    return StringUtil::subWord(getStringData(), getLength(), position, plength);
}


/**
 * Returns an array of all words contained in the given range
 * of the string, using the same extraction rules used
 * for subWord() and word().
 *
 * @param position The optional starting position.  If not provided, extraction
 *                 starts with the first word.
 * @param plength  The number of words to extract.  If omitted, will extract
 *                 from the starting postion to the end of the string.
 *
 * @return An array containing the extracted words.  If no words are
 *         available within the given range, this returns an empty
 *         array.
 */
ArrayClass *MutableBuffer::subWords(RexxInteger *position, RexxInteger *plength)
{
    return StringUtil::subWords(getStringData(), getLength(), position, plength);
}


/**
 * Extract a given word from a mutable buffer.
 *
 * @param position The target word position.
 *
 * @return The extracted word, as a string.
 */
RexxString *MutableBuffer::word(RexxInteger *position)
{
    return StringUtil::word(getStringData(), getLength(), position);
}


/**
 * return the index of a given word position in a mutable buffer
 *
 *
 * @param position The target word position.
 *
 * @return The position of the target word.
 */
RexxInteger *MutableBuffer::wordIndex(RexxInteger *position)
{
    return StringUtil::wordIndex(getStringData(), getLength(), position);
}


/**
 * return the length of a given word position in a mutable
 * buffer
 *
 *
 * @param position The target word position.
 *
 * @return The length of the target word.
 */
RexxInteger *MutableBuffer::wordLength(RexxInteger *position)
{
    return StringUtil::wordLength(getStringData(), getLength(), position);
}

/**
 * Return the count of words in the buffer.
 *
 * @return The buffer word count.
 */
RexxInteger *MutableBuffer::words()
{
    size_t tempCount = StringUtil::wordCount(getStringData(), getLength());
    return new_integer(tempCount);
}


/**
 * Perform a wordpos search on a mutablebuffer object.
 *
 * @param phrase The search phrase
 * @param pstart The starting search position.
 *
 * @return The index of the match location.
 */
RexxInteger *MutableBuffer::wordPos(RexxString  *phrase, RexxInteger *pstart)
{
    return new_integer(StringUtil::wordPos(getStringData(), getLength(), phrase, pstart));
}


/**
 * Test if the buffer contains a given word phrase.
 *
 * @param phrase The search phrase
 * @param pstart The starting search position.
 *
 * @return The index of the match location.
 */
RexxObject *MutableBuffer::containsWord(RexxString  *phrase, RexxInteger *pstart)
{
    return booleanObject(StringUtil::wordPos(getStringData(), getLength(), phrase, pstart) > 0);
}


/**
 * Perform a caseless wordpos search on a string object.
 *
 * @param phrase The search phrase
 * @param pstart The starting search position.
 *
 * @return The index of the match location.
 */
RexxInteger *MutableBuffer::caselessWordPos(RexxString  *phrase, RexxInteger *pstart)
{
    return new_integer(StringUtil::caselessWordPos(getStringData(), getLength(), phrase, pstart));
}


/**
 * Perform a caseless wordpos search on a string object.
 *
 * @param phrase The search phrase
 * @param pstart The starting search position.
 *
 * @return The index of the match location.
 */
RexxObject *MutableBuffer::caselessContainsWord(RexxString  *phrase, RexxInteger *pstart)
{
    return booleanObject(StringUtil::caselessWordPos(getStringData(), getLength(), phrase, pstart) > 0);
}


/**
 * Perform a delword operation on a mutable buffer
 *
 * @param position The position to delete.
 * @param plength  The number of words to delete
 *
 * @return Always returns the target mutable buffer.
 */
MutableBuffer *MutableBuffer::delWord(RexxInteger *position, RexxInteger *plength)
{
    size_t _wordPos = positionArgument(position, ARG_ONE);
    // default delete count is a "very large number"
    size_t count = optionalLengthArgument(plength, Numerics::MAX_WHOLENUMBER, ARG_TWO);

    // if this is a null string, we're done
    size_t length = getLength();
    if (length == 0)
    {
        return this;
    }

    // not deleting any words, also done
    if (count == 0)
    {
        return this;
    }

    // create an iterator for traversing the words
    RexxString::WordIterator iterator(getData(), length);

    // to the given word position...if we don't get there,
    // there is nothing to delete so we can just return the
    // original string.
    if (!iterator.skipWords(_wordPos))
    {
        return this;
    }

    // get the deletion point as an offset
    size_t deletePosition = iterator.wordPointer() - getStringData();

    // We're positioned at the first word we're deleting.  Skip to the last
    // word of the range, then skip over the trailing blanks.
    if (iterator.skipWords(count - 1))
    {
        iterator.skipBlanks();
    }

    size_t endPosition = iterator.scanPosition() - getStringData();

    // delete the data
    size_t gapSize = endPosition - deletePosition;
    // close up the delete part
    closeGap(deletePosition, gapSize, length - (deletePosition + gapSize));
    // adjust for the deleted data
    dataLength -= gapSize;
    return this;
}


/**
* Do an inplace space() operation on a mutable buffer.
*
* @param space_count    The number of pad characters between
*                       each word
* @param pad            The pad character
*
* @return               The target MutableBuffer
*/
MutableBuffer *MutableBuffer::space(RexxInteger *space_count, RexxString *pad)
{
    size_t count = 0;

    const size_t padLength = optionalLengthArgument(space_count, 1, ARG_ONE);
    const char   padChar   = optionalPadArgument(pad, ' ', ARG_TWO);

    // With padC the new string is not longer, so we can just overlay in place.
    // Set write position to start of buffer
    // Find first word: start position and length
    // While a word is found:
    //     Copy word to write position
    //     update write position
    //     Find next word: start position and length
    //     if no next word exists then leave
    //     select spacing count:
    //         when = 1 then append padChar and update write position
    //         when = 0 then don't pad
    //         otherwise append padC and update write position
    //     increment word interstice count
    //     iterate
    // adjust string dataLength to write position
    size_t writePos = 0;


    // get a fresh iterator for building the string
    RexxString::WordIterator iterator(getData(), getLength());

    bool haveWord = iterator.next();

    // while we still have more words, do an inplace update
    while (haveWord)
    {
        copyData(writePos, iterator.wordPointer(), iterator.wordLength());
        writePos += iterator.wordLength();

        // see if we have a following word.  If we don't we're done,
        // otherwise we need to insert the between word padding.
        haveWord = iterator.next();
        if (!haveWord)
        {
            break;
        }

        // right now, we're only padding with a single character at most.  If the
        // pad length is longer than 1, we'll handle this by expanding the gaps after
        // the string has been formatted.  More normal situations are 0 or 1 spacing.

        switch (padLength)
        {
            // single pad character, most common
            case 1:
                setData(writePos, padChar, 1);
                writePos++;
                break;
            // removing all spaces...nothing to add
            case 0:
                break;
            // spacing with multiple characters...uncommon.
            default:
                // for now, we're just going to use blanks here.  We'll make a second
                // pass and open these up after the removal process is complete.
                setData(writePos, ' ', 1);
                writePos++;
        }
        count++;
    }

    // update the data length for the new write position
    dataLength = writePos;

    // need to extend the padding?
    if (padLength > 1)
    {
        // make sure we have room for the extra
        size_t growth = count * (padLength - 1);
        ensureCapacity(growth);

        // As the string gets longer, we need to shift all data to the end and
        // then pull the pieces back in as we go.
        size_t length = getLength();
        // open a gap at the beginning of the buffer, then fill
        // with blank characters as a temporary
        openGap(0, growth, length);
        writePos = 0;
        setData(0, ' ', growth);
        // adjust the data size
        dataLength = getLength() + growth;

        // Now we do the last loop over, using padChar and padLength
        writePos = 0;

        // get a fresh iterator for building the string
        RexxString::WordIterator iterator(getData(), getLength());

        bool haveWord = iterator.next();

        // while we still have more words, do an inplace update
        while (haveWord)
        {
            copyData(writePos, iterator.wordPointer(), iterator.wordLength());
            writePos += iterator.wordLength();

            // see if we have a following word.  If we don't we're done,
            // otherwise we need to insert the between word padding.
            haveWord = iterator.next();
            if (!haveWord)
            {
                break;
            }

            // fill in the real pad characters
            setData(writePos, padChar, padLength);
            writePos += padLength;
        }
    }
    return this;
}
