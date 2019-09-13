/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2017 Rexx Language Association. All rights reserved.    */
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

#include "ooDialog.hpp"     // Must be first, includes windows.h, commctrl.h, and oorexxapi.h
#include <stdio.h>
#include <dlgs.h>
#include <limits.h>
#include <shlwapi.h>
#include <WindowsX.h>
#include "APICommon.hpp"
#include "ooShapes.hpp"
#include "oodCommon.hpp"
#include "oodMessaging.hpp"
#include "oodShared.hpp"
#include "oodMenu.hpp"


/** NOTES:
 * This approach is taken with exceptions:
 *
 * When a method is to return a new Menu object (Rexx object) always raise
 * exceptions for errors.  (Except for setting the context help ID, which will
 * not fail but set .SystemErrorCode().)
 *
 * After the Menu object is constructed, don't raise exceptions, but rather set
 * the .SystemErrorCode for failures and indicate a failure in the return.  Many
 * methods will work well with returning true of success and false for failure.
 * The OS seems to be good about accepting NULL for menu and window handles and
 * then setting the appropriate error code.
 *
 * DO raise exceptions for any invalid Rexx arguments, bad resouce ID, item ID,
 * wrong object type, wrong ranges, etc..
 *
 * Users can check for valid menu handles using isValid(), getHandle(), etc.
 *
 * When it is impossible to indicate by the return that the method failed, then
 * set .SystemErrorCode and raise an exception.  There is also the method
 * isValidItemID() which can be used to check if an itemID would raise an
 * exception  (The programmer can decide whether she wants to set up a trap
 * handler or use isValidItemID() for those methods that might raise an
 * exception.)
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 * Always use the term Popup for submenus in method names.
 *
 */


/* System Error codes that might be useful:
 *
 * ERROR_INVALID_FUNCTION          1 -
 * Incorrect function.
 *
 * ERROR_NOT_ENOUGH_MEMORY         8 -
 * Not enough storage is available to process this command.
 *
 * ERROR_INVALID_PARAMETER        87 -
 * The parameter is incorrect.
 *
 * ERROR_INVALID_WINDOW_HANDLE  1400 -
 * Invalid window handle.
 *
 * ERROR_INVALID_MENU_HANDLE    1401 -
 * Invalid menu handle.
 *
 * ERROR_WINDOW_NOT_DIALOG      1420 -
 * The window is not a valid dialog window.
 *
 * ERROR_MENU_ITEM_NOT_FOUND    1456 -
 * A menu item was not found.
 *
 * ERROR_INVALID_ENVIRONMENT    1805 -
 * The environment specified is invalid.
 *
 * OR_INVALID_OID               1911 -
 * The object specified was not found.
 *
 * ERROR_INVALID_WINDOW_STYLE   2002 -
 * The window style or class attribute is invalid for this operation.
 *
 *
 */


// Local function prototypes.
static uint32_t resolveItemID(RexxMethodContext *, RexxObjectPtr, logical_t, size_t);
static bool getMII(CppMenu *, RexxObjectPtr, BOOL, uint32_t, uint32_t *, UINT, MENUITEMINFO *);
static UINT getPopupTypeOpts(const char *, UINT);
static UINT getPopupStateOpts(const char *, UINT);
static UINT getItemStateOpts(const char *, UINT);
static UINT getItemTypeOpts(const char *, UINT);
static UINT getSeparatorTypeOpts(const char *opts, UINT type);
static UINT getTrackFlags(const char *);
static uint32_t deleteSeparatorByID(HMENU, uint32_t);
static uint32_t menuHelpID(HMENU hMenu, DWORD helpID, BOOL recurse, uint32_t *id);
static uint32_t menuConnectItems(HMENU hMenu, pCEventNotification pcen, RexxMethodContext *, CSTRING msg, bool isSysMenu);


/**
 * Calls addCommandMessage() to connect a menu item to a method in a dialog.
 *
 * addCommandMessage() only fails if the message table is full, (well also some
 * other things that are not valid here, like a zero length msg.)  So, if it
 * fails ERROR_NOT_ENOUGH_MEMORY is a good system error code.
 *
 * Note that resource IDs, 1, 2, and 9 are connected automaticaly when a dialog
 * is initialized.
 *
 * @param pcen  EventNotification CSelf for the dialog we are adding the method
 *              connection to.
 *
 * @param c     Rexx method context we are operating in.
 *
 * @param id    Menu ID to connect.
 *
 * @param msg   Method name we are connecting to.
 *
 * @return 0 on success, ERROR_NOT_ENOUGH_MEMORY on failure.
 *
 * @remarks The WPARAM filter (menu id filter) needs to be 0xFFFFFFFF.  This
 *          filters out dialog control notifications that use WM_COMMAND.  The
 *          high word of WPARAM is the notification code for dialog controls, it
 *          is 0 for menu command items.
 */
inline uint32_t _connectItem(pCEventNotification pcen, RexxMethodContext *c, uint32_t id, CSTRING msg)
{
    if ( id < 3 || id == 9 )
    {
        return 0;
    }
    return addCommandMessage(pcen, c, id, 0xFFFFFFFF, 0, 0, msg, TAG_NOTHING) ? 0 : ERROR_NOT_ENOUGH_MEMORY;
}

/* Same as above but connects a System Menu item */
inline BOOL _connectSysItem(pCEventNotification pcen, RexxMethodContext *c, uint32_t id, CSTRING msg)
{
    uint32_t tag = TAG_MENU;
    return addMiscMessage(pcen, c, WM_SYSCOMMAND, UINT32_MAX, id, 0x0000FFFF, 0, 0, msg, tag) ? 0 : ERROR_NOT_ENOUGH_MEMORY;
}


/**
 * Checks if CppMenu::attachToDlg() would succeed with the specified object.
 *
 * @param c
 * @param dlg
 * @param argPos
 *
 * @return true if attachToDlg() is expected to succeed, otherwise false.
 *
 * @note If false is returned, an exception has been raised.
 */
static bool validAttachTo(RexxMethodContext *c, RexxObjectPtr dlg, size_t argPos)
{
    TCHAR buf[256];

    if ( ! c->IsOfType(dlg, "PLAINBASEDIALOG") )
    {
        _snprintf(buf, sizeof(buf), "can not attach menu unless arg %zd 'attachTo' is a dialog object", argPos);
        userDefinedMsgException(c, buf);
        return false;
    }

    // The underlying dialog needs to exist, and it can not already have a menu
    // bar attached.
    pCPlainBaseDialog pcpbd = dlgToCSelf(c, dlg);

    if ( ! pcpbd->isDlgHwndSet )
    {
        _snprintf(buf, sizeof(buf), "can not attach menu when the underlying arg %zd 'attachTo' dialog does not exist",
                  argPos);
        userDefinedMsgException(c, buf);
        return false;
    }

    if ( c->SendMessage0(dlg, "HASMENUBAR") == TheTrueObj )
    {
        _snprintf(buf, sizeof(buf), "can not attach menu when the arg %zd 'attachTo' dialog already has a menu bar",
                  argPos);
        userDefinedMsgException(c, buf);
        return false;
    }

    return true;
}

static RexxObjectPtr sc2rexx(RexxThreadContext *c, WPARAM wParam)
{
    CSTRING s;

    switch ( wParam & 0xFFF0 )
    {
        case SC_SIZE :
            s = "SIZE";
            break;
        case SC_MOVE :
            s = "MOVE";
            break;
        case SC_MINIMIZE :
            s = "MINIMIZE";
            break;
        case SC_MAXIMIZE :
            s = "MAXIMIZE";
            break;
        case SC_NEXTWINDOW   :
            s = "NEXTWINDOW";
            break;
        case SC_PREVWINDOW   :
            s = "PREVWINDOW";
            break;
        case SC_CLOSE :
            s = "CLOSE";
            break;
        case SC_VSCROLL :
            s = "VSCROLL";
            break;
        case SC_HSCROLL :
            s = "HSCROLL";
            break;
        case SC_MOUSEMENU :
            s = "MOUSEMENU ";
            break;
        case SC_KEYMENU :
            s = "KEYMENU";
            break;
        case SC_ARRANGE :
            s = "ARRANGE";
            break;
        case SC_RESTORE :
            s = "RESTORE";
            break;
        case SC_TASKLIST :
            s = "TASKLIST";
            break;
        case SC_SCREENSAVE :
            s = "SCREENSAVE";
            break;
        case SC_HOTKEY :
            s = "HOTKEY";
            break;
        case SC_DEFAULT :
            s = "DEFAULT";
            break;
        case SC_MONITORPOWER :
            s = "MONITORPOWER";
            break;
        case SC_CONTEXTHELP :
            s = "CONTEXTHELP";
            break;
        case SC_SEPARATOR :
            s = "SEPARATOR";
            break;

        // SCF_ISSECURE, only defined if WINVER >= 0x0600
        case 0x00000001 :
            s = "ISSECURE";
            break;
        default :
            // Could be a menu command item inserted by the user
            return c->WholeNumber(wParam & 0xFFF0);
    }
    return c->String(s);
}

/**
 * Tries to get a pointer to the EventNotification CSelf from the specified
 * dialog.
 *
 * @param c       Method context we are operating in.
 * @param dialog  A presumed ooDialog dialog object.
 *
 * @return The EventNotification CSelf for the specified dialog on success, null
 *         on failure.
 *
 * @note   This function fails if dialog is not a PlainBaseDialog or one of its
 *         subclasses.  In this case the .SystemErrorCode is set, but no
 *         condition is raised.
 */
static pCEventNotification _getPCEN(RexxMethodContext *c, RexxObjectPtr dialog)
{
    oodResetSysErrCode(c->threadContext);

    if ( dialog != NULLOBJECT && c->IsOfType(dialog, "PLAINBASEDIALOG") )
    {
        return dlgToEventNotificationCSelf(c, dialog);
    }

    oodSetSysErrCode(c->threadContext, ERROR_WINDOW_NOT_DIALOG);
    return NULL;
}

/**
 * Handles a menu event notification message.  Invoked from searchMiscTable().
 *
 * @param pcpbd
 * @param wmMsg
 * @param wParam
 * @param lParam
 * @param method
 * @param tag
 *
 * @return MsgReplyType
 */
MsgReplyType processMenuMsg(pCPlainBaseDialog pcpbd, uint32_t wmMsg, WPARAM wParam, LPARAM lParam, CSTRING method, uint32_t tag)
{
    RexxThreadContext *c = pcpbd->dlgProcContext;
    RexxArrayObject    args;
    MsgReplyType       reply = ReplyFalse;

    switch ( wmMsg )
    {
        case WM_CONTEXTMENU :
        {
            /* On WM_CONTEXTMENU, if the message is
             * generated by the keyboard (say SHIFT-F10)
             * then the x and y coordinates are sent as -1
             * and -1. Args to ooRexx: hwnd, x, y
             *
             * Note that the current context menu processing is
             * dependent on the event handler *not* running in
             * the window message processing loop.  So
             * inovkeDispatch() is required.  If this is
             * changed, then the code using WM_USER_CONTEXT_MENU
             * needs to be reviewed.
             */
            RexxObjectPtr hwndClickedOn = pointer2string(c, (void *)wParam);
            RexxObjectPtr x             = c->Int32(GET_X_LPARAM(lParam));
            RexxObjectPtr y             = c->Int32(GET_Y_LPARAM(lParam));

            args = c->ArrayOfThree(hwndClickedOn, x, y);
            invokeDispatch(c, pcpbd, method, args);

            c->ReleaseLocalReference(hwndClickedOn);
            c->ReleaseLocalReference(x);
            c->ReleaseLocalReference(y);
            c->ReleaseLocalReference(args);

            return ReplyTrue;
        }
        break;

        case WM_MENUCOMMAND :
        {
            /* Args to ooRexx: one-based index of item selected, hMenu.
             *
             * TODO we should send the Rexx Menu object rather than the handle.
             * This would invole constructing the object, or grabbing it from
             * the Menu user words.
             *
             * Note that we don't actually connect this message, yet.  We only
             * connect WM_COMMAND for menus.
             */
            RexxObjectPtr index = c->WholeNumber(wParam + 1);
            RexxObjectPtr hMenu = c->NewPointer((POINTER)lParam);

            args = c->ArrayOfTwo(index, hMenu);
            invokeDispatch(c, pcpbd, method, args);

            c->ReleaseLocalReference(index);
            c->ReleaseLocalReference(hMenu);
            c->ReleaseLocalReference(args);

            return ReplyTrue;
        }
        break;

        case WM_SYSCOMMAND :
        {
            /* Args to ooRexx: The SC_xx command name, x, y
             */
            RexxObjectPtr sc_cmd = sc2rexx(c, wParam);
            RexxObjectPtr x, y;
            bool          needRelease = false;

            if ( GET_Y_LPARAM(lParam) == -1 )
            {
                x = TheNegativeOneObj;
                y = TheNegativeOneObj;
            }
            else if ( GET_Y_LPARAM(lParam) == 0 )
            {
                x = TheZeroObj;
                y = TheZeroObj;
            }
            else
            {
                x = c->Int32(GET_X_LPARAM(lParam));
                y = c->Int32(GET_Y_LPARAM(lParam));
                needRelease = true;
            }

            args = c->ArrayOfThree(sc_cmd, x, y);
            RexxObjectPtr msgReply = c->SendMessage(pcpbd->rexxSelf, method, args);

            msgReply = requiredBooleanReply(c, pcpbd, msgReply, method, false);
            if ( msgReply == TheTrueObj )
            {
                setWindowPtr(pcpbd->hDlg, DWLP_MSGRESULT, 0);
                reply = ReplyTrue;
            }
            else
            {
                setWindowPtr(pcpbd->hDlg, DWLP_MSGRESULT, 1);
            }

            if ( needRelease )
            {
                c->ReleaseLocalReference(x);
                c->ReleaseLocalReference(y);
            }
            c->ReleaseLocalReference(sc_cmd);
            c->ReleaseLocalReference(args);

            return reply;
        }
        break;

        case WM_INITMENU :
        {
            // Args to ooRexx: hMenu as a pointer. TODO would really be nice to
            // send the Rexx menu object itself instead of the handle.
            RexxPointerObject rxHMenu = c->NewPointer((POINTER)wParam);
            RexxObjectPtr     msgReply;

            args     = c->ArrayOfOne(rxHMenu);
            msgReply = c->SendMessage(pcpbd->rexxSelf, method, args);
            msgReply = requiredBooleanReply(c, pcpbd, msgReply, method, false);

            if ( msgReply == TheTrueObj )
            {
                setWindowPtr(pcpbd->hDlg, DWLP_MSGRESULT, 0);
                reply = ReplyTrue;
            }
            else
            {
                setWindowPtr(pcpbd->hDlg, DWLP_MSGRESULT, 1);
            }
            c->ReleaseLocalReference(rxHMenu);
            c->ReleaseLocalReference(args);

            return reply;
        }
        break;

        case WM_INITMENUPOPUP :
        {
            // Args to ooRexx: 1-based position index of the item that opens the
            // drop down menu or submenu, isSystemMenu, hMenu as a pointer. TODO
            // would really be nice to send the Rexx menu object itself.
            RexxPointerObject rxHMenu   = c->NewPointer((POINTER)wParam);
            RexxObjectPtr     posIndex  = c->Int32(LOWORD(lParam) + 1);
            RexxObjectPtr     isSysMenu = c->Logical(HIWORD(lParam));
            RexxObjectPtr     msgReply;

            args = c->ArrayOfThree(posIndex, isSysMenu, rxHMenu);
            msgReply = c->SendMessage(pcpbd->rexxSelf, method, args);
            msgReply = requiredBooleanReply(c, pcpbd, msgReply, method, false);

            if ( msgReply == TheTrueObj )
            {
                setWindowPtr(pcpbd->hDlg, DWLP_MSGRESULT, 0);
                reply = ReplyTrue;
            }
            else
            {
                setWindowPtr(pcpbd->hDlg, DWLP_MSGRESULT, 1);
            }
            c->ReleaseLocalReference(rxHMenu);
            c->ReleaseLocalReference(posIndex);
            c->ReleaseLocalReference(isSysMenu);
            c->ReleaseLocalReference(args);

            return reply;
        }
        break;

        default :
            break;
    }

    return reply;
}


CSTRING CppMenu::name()
{
    switch ( type )
    {
        case BinaryMenuBar :
            return "BinaryMenuBar";
        case PopupMenu :
            return "PopupMenu";
        case ScriptMenuBar :
            return "ScriptMenuBar";
        case UserMenuBar :
            return "UserMenuBar";
        default :
            break;
    }
    return "ERROR unknown menu";
}

logical_t CppMenu::addTemplateSepartor(RexxObjectPtr rxID, CSTRING opts)
{
    logical_t success = FALSE;
    oodResetSysErrCode(c->threadContext);

    int32_t id = oodGlobalID(c, rxID, 1, false);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    if ( isFinal )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    WORD resInfo = 0;
    DWORD dwType = MFT_SEPARATOR;

    if ( opts != NULL )
    {
        dwType = getSeparatorTypeOpts(opts, dwType);
        if ( StrStrI(opts, "END") )
        {
            resInfo = MFR_END;
        }
    }
    success = addTemplateMenuItem(id, dwType, 0, 0, resInfo, "");

done_out:
    return success;
}


/**
 *
 *
 * @param rxID
 * @param text
 * @param opts
 * @param method
 *
 * @return logical_t
 *
 * @note text is required to be not null, the empty string is okay.
 */
logical_t CppMenu::addTemplateItem(RexxObjectPtr rxID, CSTRING text, CSTRING opts, CSTRING method)
{
    logical_t success = FALSE;
    oodResetSysErrCode(c->threadContext);

    int32_t id = oodGlobalID(c, rxID, 1, true);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    if ( isFinal )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    if ( method != NULL && *method == '\0' )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
        goto done_out;
    }

    WORD resInfo = 0;
    DWORD dwState = 0;
    DWORD dwType = 0;

    if ( opts != NULL )
    {
        dwState = getItemStateOpts(opts, 0);
        dwType = getItemTypeOpts(opts, MFT_STRING);
        if ( StrStrI(opts, "END") )
        {
            resInfo = MFR_END;
        }
    }

    if ( ! addTemplateMenuItem(id, dwType, dwState, 0, resInfo, text) )
    {
        goto done_out;
    }

    success = TRUE;

    if ( method != NULL )
    {
        addToConnectionQ(id, method);
    }

done_out:
    return success;
}


/**
 *
 *
 * @param rxID
 * @param text
 * @param opts
 * @param helpID
 *
 * @return logical_t
 *
 * @note Text is required to not be null, the empty string is okay.
 *
 * @remarks  We need to allow 0 for rxID, so don't use 'strict' with
 *           oodGlobalID().
 */
logical_t CppMenu::addTemplatePopup(RexxObjectPtr rxID, CSTRING text, CSTRING opts, RexxObjectPtr helpID)
{
    logical_t success = FALSE;
    oodResetSysErrCode(c->threadContext);

    int32_t id = oodGlobalID(c, rxID, 1, false);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    DWORD dwHelpID = 0;
    if ( helpID != NULLOBJECT )
    {
        dwHelpID = oodGlobalID(c, helpID, 4, false);
        if ( id == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
    }

    if ( isFinal )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    WORD resInfo = MFR_POPUP;
    DWORD dwState = 0;
    DWORD dwType = 0;

    if ( opts != NULL )
    {
        dwState = getPopupStateOpts(opts, 0);
        dwType = getPopupTypeOpts(opts, MFT_STRING);
        if ( StrStrI(opts, "END") )
        {
            resInfo |= MFR_END;
        }
    }
    success = addTemplateMenuItem(id, dwType, dwState, dwHelpID, resInfo, text);

done_out:
    return success;
}


bool CppMenu::initTemplate(uint32_t count, uint32_t _helpID)
{
    WORD *p;
    bool success = false;

    helpID = _helpID;

    // Allocate the memory for the template, which is a guess based on the
    // expected count of menu items.  We add 1 to account for the header and use
    // an arbitrarily picked menu item size.  (Which is probably much bigger
    // than a typical menu item size.
    size_t size = (++count) * ARBITRARY_MENU_ITEM_SIZE;
    hTemplateMemory = (PDWORD)GlobalAlloc(GPTR, size);
    if ( hTemplateMemory == NULL )
    {
        systemServiceExceptionCode(c->threadContext, API_FAILED_MSG, "GlobalAlloc");
        goto done_out;
    }

    p = (PWORD)hTemplateMemory;

    isFinal = false;
    endOfTemplate = (byte *)p + size - 1;

    /* Write the menu header template.  Extended menu headers and extended menu
     * item templates must be DWORD aligned. Begin by aligning the starting
     * pointer.  Note that lpwAlign aligns to a DWORD, not a word.  (Haven't
     * changed the name because it is used in other, non-reviewed, code.
     */
    p = lpwAlign(p);

    // Save the start of the template so we can load it when we are done.
    pTemplate = (DWORD *)p;

    *p++ = EXTENDED_MENU_VERSION;  // wVersion (word): 1 for extended menu.
    *p++ = 4;                      // wOffset (word):  4 since we are already aligned.

    /* The context help ID is a DWORD value.  Note that setting the help ID does
     * not seem to work as Microsoft documents it.  After this, our current
     * position pointer ends up pointing to the start of the 1st menu item,
     * already aligned.
     */
    pCurrentTemplatePos = (DWORD *)p;
    *pCurrentTemplatePos++ = helpID;

    success = true;

done_out:
    return success;
}


/**
 * Adds a menu item to the in memory template.
 *
 * @param menuID
 * @param dwType
 * @param dwState
 * @param dwHelpID
 * @param resInfo
 * @param text      Must not be null, use the empty string.
 *
 * @return bool
 *
 * @assumes Caller has checked that the memory template state is valid.
 */
BOOL CppMenu::addTemplateMenuItem(DWORD menuID, DWORD dwType, DWORD dwState, DWORD dwHelpID, WORD resInfo, CSTRING text)
{
    WORD *p;
    BOOL  success = TRUE;

    if ( ! haveTemplateRoom(strlen(text) + 1, (byte *)pCurrentTemplatePos) )
    {
        executionErrorException(c->threadContext, TEMPLATE_TOO_SMALL_MSG);
        return FALSE;
    }

    // Our current position pointer is DWORD aligned pointing to the next menu
    // item to add.

    *pCurrentTemplatePos++ = dwType;
    *pCurrentTemplatePos++ = dwState;
    *pCurrentTemplatePos++ = menuID;

    // Next fields are WORD in size.
    p = (WORD *)pCurrentTemplatePos;

    *p++ = resInfo;

    // The menu item strings must be unicode.  This works correctly for the
    // empty string.  p now points to the position in the template for the
    // string.  If there is no string, then this position still needs to be set
    // with the wide character null.  putUnicodeText() will handle both the
    // empty string or text == NULL.
    p += putUnicodeText(p, text);

    // Need to be double word aligned now.
    pCurrentTemplatePos = (DWORD *)lpwAlign(p);

    // The dwHelpId field must be included, even if it is 0, if the item is a
    // popup, must not be included otherwise.
    if (resInfo & MFR_POPUP)
    {
        *pCurrentTemplatePos++ = dwHelpID;
    }

    return success;
}

void CppMenu::deleteTemplate()
{
    GlobalFree(hTemplateMemory);

    isFinal = true;
    pTemplate = NULL;
    pCurrentTemplatePos = NULL;
    hTemplateMemory = NULL;
}


bool CppMenu::finishTemplate()
{
    bool result = false;
    oodResetSysErrCode(c->threadContext);

    if ( isFinal )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    // Load the menu and save last error to be sure it does not get changed by
    // some internal Windows API while we are doing clean up.
    hMenu = LoadMenuIndirect(pTemplate);
    uint32_t rc = GetLastError();

    // No matter what, we are done with the memory allocated for the menu
    // template and we set ourself to finialized.
    deleteTemplate();

    if ( hMenu == NULL )
    {
        systemServiceExceptionCode(c->threadContext, API_FAILED_MSG, "LoadMenuIndirect", rc);
        goto done_out;
    }
    result = true;

    uint32_t ret = menuHelpID(hMenu, helpID, TRUE, NULL);
    if ( ret != 0 )
    {
        oodSetSysErrCode(c->threadContext, ret);
    }

done_out:
    noTempHelpID();
    return result;
}

CppMenu::~CppMenu()
{
    printf("In destructor connectionQ=%p\n", connectionQ);
}

CppMenu::CppMenu(RexxObjectPtr s, MenuType t, RexxMethodContext *context) : self(s), type(t), c(context)
{
    defaultResult = NULLOBJECT;
    hMenu = NULL;
    wID = -1;
    dlg = TheNilObj;
    dlgHwnd = NULL;

    connectionRequested = false;
    connectionQ = NULL;
    connectionQIndex = 0;
    connectionQSize = 0;

    autoConnect = false;
    connectionMethod = NULL;

    hTemplateMemory = NULL;
    pTemplate = NULL;
    pCurrentTemplatePos = NULL;
    isFinal = true;
    uint32_t helpID = (uint32_t)-1;
}


/**
 *  Sets the menu ID for this menu.  The default ID of -1 is already set.
 *
 *  @param  menuID   The ID of the menu, or 0 or -1 to signal no ID.
 *
 *  @return No return.
 *
 *  @remarks  A menu ID was not used in earlier versions of ooDialog and it is
 *  not clear if ooDialog programmers will use it much now that it is available.
 *  wID is already set to -1 to signal no ID when the CppMenu is constructed.
 *  So this method only needs to be called if the programmer does use an ID.
 *
 *  This could be inline of course, just saving this for the remarks.
 */
void CppMenu::setMenuID(int menuID)
{
    if ( menuID > 0 )
    {
        wID = menuID;
    }
}


RexxDirectoryObject CppMenu::autoConnectionStatus()
{
    RexxDirectoryObject result = c->NewDirectory();

    c->DirectoryPut(result, autoConnect ? TheTrueObj : TheFalseObj, "AUTOCONNECT");
    if ( connectionMethod != NULL )
    {
        c->DirectoryPut(result, c->CString(connectionMethod), "METHODNAME");
    }
    return result;
}

/**
 *  Given the arguments and the current state of the menu, determines if the
 *  programmer wants the menu item automatically connected to a method.
 *
 * @param id          The id of the menu item.
 * @param text        The menu item text (label.)
 * @param connect     True if programmer requested the item to be connected.
 * @param methodName  Optional method name for connection request, may be NULL.
 *                    If null, use a method name constructed from the text of
 *                    the item
 *
 * @return True if no errors, if either the item was connected or there was no
 *         request to connect the item.  Return false if the item was suppossed
 *         to be connected, but an error ocurred.
 *
 * @note  We connect the item under two circumstances: 1.) connect is true, the
 *        programmer requested it.  2.) Autoconnection is on and the menu is
 *        attached to a dialog.
 *
 *        If the programmer has both turned on Autoconnection and requests the
 *        item be connected, then the outcome is undefined.  The programmer
 *        shouldn't do that.
 *
 * @assumes id is a resource ID and not a by position ID.  This menu is a menu
 *          bar.
 */
BOOL CppMenu::maybeConnectItem(uint32_t id, CSTRING text, logical_t connect, CSTRING methodName)
{
    BOOL success = TRUE;
    char * _methodName = NULL;
    bool doAutoConnection = autoConnect && (dlg != TheNilObj);

    if ( ! connect && ! doAutoConnection )
    {
        goto done_out;
    }

    success = FALSE;
    uint32_t rc = 0;

    pCPlainBaseDialog pcpbd = NULL;
    if ( dlg != TheNilObj )
    {
        pcpbd = dlgToCSelf(c, dlg);
    }

    if ( doAutoConnection )
    {
        if ( connectionMethod == NULL )
        {
            _methodName = strdup_2methodName(text);
            if ( _methodName == NULL )
            {
                oodSetSysErrCode(c->threadContext, ERROR_NOT_ENOUGH_MEMORY);
                goto done_out;
            }

            // Method name can not be the empty string.
            if ( *_methodName == '\0' )
            {
                oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
                goto done_out;
            }
        }
        else if ( *connectionMethod == '\0' )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
            goto done_out;
        }

        rc = _connectItem(pcpbd->enCSelf, c, id, connectionMethod == NULL ? _methodName : connectionMethod);
        if ( rc != 0 )
        {
            oodSetSysErrCode(c->threadContext, rc);
            goto done_out;
        }
    }

    if ( connect )
    {
        if ( methodName == NULL )
        {
            // We may have already constructed _methodName ...
            if ( _methodName == NULL )
            {
                _methodName = strdup_2methodName(text);
                if ( _methodName = NULL )
                {
                    oodSetSysErrCode(c->threadContext, ERROR_NOT_ENOUGH_MEMORY);
                    goto done_out;
                }

                // Method name can not be the empty string.
                if ( *_methodName == '\0' )
                {
                    oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
                    goto done_out;
                }
            }
            methodName = _methodName;
        }
        else if ( *methodName == '\0' )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
            goto done_out;
        }

        // If we have an owner dialog, connect the menu item now, otherwise
        // add it to the connection queue.
        if ( pcpbd != NULL )
        {
            rc = _connectItem(pcpbd->enCSelf, c, id, methodName);
            if ( rc != 0 )
            {
                oodSetSysErrCode(c->threadContext, rc);
                goto done_out;
            }
        }
        else
        {
            if ( ! addToConnectionQ(id, methodName) )
            {
                // Couldn't allocate memory
                goto done_out;
            }
        }
    }
    success = TRUE;

done_out:
    safeFree(_methodName);
    return success;
}


/**
 * Adds a menu command item to the connection queue so that the command event
 * can be connected to a dialog at a later time.
 *
 * @param id
 * @param methodName
 *
 * @return bool
 *
 * @remarks  Default menu item count is 100, which should be adequate most of
 *           the time.  The idea was to increase the queue using GlobalReAlloc()
 *           if a programmer is using more menu items.
 *
 *           However, in a real world program where the menu items equal 114,
 *           GlobalReAlloc() always returns null at the point the queue needs to
 *           be increased.  Testing shows that GlobalAlloc() succeeds in
 *           allocating the new memory amount.  Not sure if that is user error,
 *           or what.  So, for now, we just do it the hard way.
 */
bool CppMenu::addToConnectionQ(uint32_t id, CSTRING methodName)
{
    bool result = false;

    // If we have never had a connection request, allocate the connecion queue.
    if ( ! connectionRequested )
    {
        connectionQ = (MapItem **)GlobalAlloc(GPTR, DEFAULT_MENUITEM_COUNT * sizeof(MapItem *));
        if ( connectionQ == NULL )
        {
            outOfMemoryException(c->threadContext);
            goto done_out;
        }
        connectionQSize = DEFAULT_MENUITEM_COUNT;
        connectionQIndex = 0;
        connectionRequested = true;
    }

    // If the connection queue is full, double its size.
    if ( connectionQIndex >= connectionQSize )
    {
        MapItem **pNew = (MapItem **)GlobalAlloc(GPTR, 2 * connectionQSize * sizeof(MapItem *));
        if ( pNew == NULL )
        {
            releaseConnectionQ();
            outOfMemoryException(c->threadContext);
            goto done_out;
        }
        memmove(pNew, connectionQ, connectionQSize * sizeof(MapItem *));
        GlobalFree(connectionQ);
        connectionQ = pNew;
        connectionQSize = 2 * connectionQSize;
    }

    // Add the new item to the connection queue.
    MapItem *m = (MapItem *)GlobalAlloc(GMEM_FIXED, sizeof(MapItem));
    if ( m == NULL )
    {
        releaseConnectionQ();
        outOfMemoryException(c->threadContext);
        goto done_out;
    }

    m->id = id;
    m->methodName = (char *)GlobalAlloc(GMEM_FIXED, strlen(methodName) + 1);
    if ( m->methodName == NULL )
    {
        GlobalFree(m);
        releaseConnectionQ();
        outOfMemoryException(c->threadContext);
        goto done_out;
    }
    strcpy(m->methodName, methodName);
    connectionQ[connectionQIndex++] = m;
    result = true;

done_out:
    return result;
}

void CppMenu::releaseConnectionQ()
{
    if ( connectionQ == NULL )
    {
        return;
    }

    MapItem *m;
    for ( size_t i = 0; i < connectionQIndex; i++ )
    {
        m = connectionQ[i];
        if ( m != NULL )
        {
            if ( m->methodName != NULL )
            {
                GlobalFree(m->methodName);
            }
            GlobalFree(m);
        }
    }
    GlobalFree(connectionQ);
    connectionQ = NULL;
    connectionQSize = 0;
    connectionQIndex = 0;
    connectionRequested = false;
}


logical_t CppMenu::attachToDlg(RexxObjectPtr dialog, uint32_t countRows)
{
    oodResetSysErrCode(c->threadContext);

    // Check all of our required conditions.
    if ( hMenu == NULL )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_MENU_HANDLE);
        goto no_change;
    }
    if ( ! c->IsOfType(dialog, "PLAINBASEDIALOG") )
    {
        oodSetSysErrCode(c->threadContext, ERROR_WINDOW_NOT_DIALOG);
        goto no_change;
    }
    if ( dlg != TheNilObj )
    {
        // Already attached to a dialog.
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
        goto no_change;
    }

    pCPlainBaseDialog pcpbd = dlgToCSelf(c, dialog);

    if ( pcpbd->hDlg == NULL )
    {
        // Underlying Windows dialog has not been created.
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_WINDOW_HANDLE);
        goto no_change;
    }

    if ( c->SendMessage0(dialog, "HASMENUBAR") == TheTrueObj || GetMenu(pcpbd->hDlg) != NULL )
    {
        // Dialog already has a menu attached.  User needs to remove it first.
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_WINDOW_STYLE);
        goto no_change;
    }

    RECT r;
    uint32_t flags = SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOREDRAW | SWP_NOMOVE;

    if ( countRows > 0 )
    {
        GetWindowRect(pcpbd->hDlg, &r);

        int32_t newHeight = (r.bottom - r.top) + (countRows * GetSystemMetrics(SM_CYMENU));
        SetWindowPos(pcpbd->hDlg, 0, r.left, r.top, r.right - r.left, newHeight, flags);
    }

    if ( SetMenu(pcpbd->hDlg, hMenu) == 0 )
    {
        SetWindowPos(pcpbd->hDlg, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, flags);
        oodSetSysErrCode(c->threadContext);
        goto no_change;
    }

    dlg = dialog;
    dlgHwnd = pcpbd->hDlg;

    c->SendMessage1(dlg, "LINKMENU", self);
    return checkPendingConnections();

no_change:
    return FALSE;
}


logical_t CppMenu::assignToDlg(RexxObjectPtr dialog, logical_t _autoConnect, CSTRING methodName)
{
    oodResetSysErrCode(c->threadContext);
    logical_t success = FALSE;

    if ( hMenu == NULL )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_MENU_HANDLE);
        goto done_out;
    }
    if ( ! c->IsOfType(dialog, "PLAINBASEDIALOG") )
    {
        oodSetSysErrCode(c->threadContext, ERROR_WINDOW_NOT_DIALOG);
        goto done_out;
    }

    // We don't care if there is already an assigned dialog, we just replace it.
    // But, we want to be sure we don't half way replace it.

    pCPlainBaseDialog pcpbd = dlgToCSelf(c, dialog);

    if ( pcpbd->hDlg == NULL )
    {
        // Underlying Windows dialog has not been created.
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_WINDOW_HANDLE);
        goto done_out;
    }

    dlg = dialog;
    dlgHwnd = pcpbd->hDlg;

    // _autoConnect could be 0 if it was omitted, or if the programmer actually
    // passed in .false.
    if ( argExists(2) )
    {
        if ( ! setAutoConnection(_autoConnect, methodName) )
        {
            goto done_out;
        }
    }

    success = checkAutoConnect(pcpbd->enCSelf);

done_out:
    return success;
}

/**
 * Detaches this menu bar from its owner dialog and sets the state of both the
 * dialog and this menu bar to not attached.
 *
 * @param  skipChecks  If true don't bother checking for an attached dialog, the
 *                     check has already been done.
 *
 * @return true on success and false on failure.
 *
 * @note Resets .SystemErrorCode.
 */
BOOL CppMenu::detach(bool skipChecks)
{
    oodResetSysErrCode(c->threadContext);
    BOOL success = FALSE;

    if ( ! skipChecks )
    {
        if ( ! isMenuBar() || dlg == TheNilObj )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
            goto done_out;
        }
    }

    success = SetMenu(dlgHwnd, NULL);
    if ( ! success )
    {
        oodSetSysErrCode(c->threadContext);
    }
    c->SendMessage0(dlg, "UNLINKMENU");
    dlg = TheNilObj;

done_out:
    return success;
}

/**
 * Replaces this menu bar attached to its owner dialog with a new menu bar.
 *
 * @param  newMenuBar  The replacement menu bar.
 *
 * @return The old menu bar on success, or .nil on error
 *
 * @note Resets .SystemErrorCode.
 */
RexxObjectPtr CppMenu::replace(RexxObjectPtr newMenuBar)
{
    oodResetSysErrCode(c->threadContext);
    RexxObjectPtr oldMenuBar = TheNilObj;

    if ( ! c->IsOfType(newMenuBar, "MENUBAR") )
    {
        wrongClassException(c->threadContext, 1, "MenuBar");
        goto done_out;
    }

    if ( ! isAttached() )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    CppMenu *cMenubar = (CppMenu *)c->ObjectToCSelf(newMenuBar);

    if ( SetMenu(dlgHwnd, cMenubar->getHMenu()) == 0 )
    {
        oodSetSysErrCode(c->threadContext);
        goto done_out;
    }

    oldMenuBar = c->SendMessage1(dlg, "LINKMENU", cMenubar->getSelf());
    maybeRedraw(false);

    pCEventNotification pcen = dlgToEventNotificationCSelf(c, dlg);

    cMenubar->checkAutoConnect(pcen);

done_out:
    return oldMenuBar;
}

/**
 * Called from the Rexx uninit() method.
 *
 */
void CppMenu::uninitMenu()
{
    if ( connectionMethod != NULL )
    {
        LocalFree(connectionMethod);
        connectionMethod = NULL;
    }

    if ( isMenuBar() && dlg != TheNilObj && hMenu != NULL )
    {
        DestroyMenu(hMenu);
    }
}

/**
 * Destroys the underlying menu.
 *
 * Normally this would never be called when the menu is attached to a dialog,
 * but if it is attached, it is first detached (any errors from detach() are
 * just ignored.)
 *
 * @return True on success, false on failure.
 *
 * @note Resets .SystemErrorCode.
 */
BOOL CppMenu::destroy()
{
    if ( isMenuBar() && dlg != TheNilObj )
    {
        detach(true);
    }
    oodResetSysErrCode(c->threadContext);

    BOOL success = DestroyMenu(hMenu);
    if ( ! success )
    {
        oodSetSysErrCode(c->threadContext);
    }
    hMenu = NULL;
    releaseConnectionQ();

    // TODO any other clean up need to be done here ???

    return success;
}

logical_t CppMenu::setMaxHeight(uint32_t height, logical_t recurse)
{
    logical_t success = FALSE;
    oodResetSysErrCode(c->threadContext);

    MENUINFO mi = {0};
    mi.cbSize = sizeof(MENUINFO);
    mi.fMask = MIM_MAXHEIGHT;
    mi.cyMax = height;
    if ( recurse )
    {
        mi.fMask |= MIM_APPLYTOSUBMENUS;
    }

    success = SetMenuInfo(hMenu, &mi);
    if ( ! success )
    {
        oodSetSysErrCode(c->threadContext);
    }
    return success;
}

int CppMenu::getMaxHeight()
{
    int result = -1;
    oodResetSysErrCode(c->threadContext);

    MENUINFO mi = {0};
    mi.cbSize = sizeof(MENUINFO);
    mi.fMask = MIM_MAXHEIGHT;

    if ( GetMenuInfo(hMenu, &mi) == 0 )
    {
        oodSetSysErrCode(c->threadContext);
    }
    else
    {
        result = mi.cyMax;
    }
    return result;
}

void CppMenu::putSysCommands()
{
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF000), "SC_SIZE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF010), "SC_MOVE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF020), "SC_MINIMIZE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF030), "SC_MAXIMIZE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF040), "SC_NEXTWINDOW");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF050), "SC_PREVWINDOW");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF060), "SC_CLOSE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF070), "SC_VSCROLL");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF080), "SC_HSCROLL");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF090), "SC_MOUSEMENU");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF100), "SC_KEYMENU");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF110), "SC_ARRANGE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF120), "SC_RESTORE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF130), "SC_TASKLIST");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF140), "SC_SCREENSAVE");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF150), "SC_HOTKEY");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF160), "SC_DEFAULT");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF170), "SC_MONITORPOWER");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF180), "SC_CONTEXTHELP");
    c->DirectoryPut(TheConstDir, c->UnsignedInt32(0xF00F), "SC_SEPARATOR");
}

/**
 * Finishes up the .SystemMenu init().
 *
 * @param dialog  The Rexx dialog object.
 *
 * @return bool
 *
 * @remarks  The caller has ensured that dialog is a .PlainBaseDialog.  We
 *           ensure that the underlying Windows dialog has been created.
 */
bool CppMenu::setUpSysMenu(RexxObjectPtr dialog)
{
    oodResetSysErrCode(c->threadContext);
    bool success = false;

    pCPlainBaseDialog pcpbd = dlgToCSelf(c, dialog);

    if ( pcpbd->hDlg == NULL )
    {
        failedToRetrieveException(c->threadContext, "window handle", dialog);
        goto done_out;
    }

    hMenu = GetSystemMenu(pcpbd->hDlg, FALSE);
    if ( hMenu == NULL || ! IsMenu(hMenu) )
    {
        hMenu = NULL;
        failedToRetrieveException(c->threadContext, "system menu", dialog);
        goto done_out;
    }

    dlg = dialog;
    dlgHwnd = pcpbd->hDlg;

    putSysCommands();
    success = true;

    // TODO need to think about putting this Rexx object in the data word of the menu.

done_out:
    return success;
}

logical_t CppMenu::revertSysMenu()
{
    oodResetSysErrCode(c->threadContext);
    logical_t success = FALSE;

    if ( hMenu == NULL )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_MENU_HANDLE);
        goto done_out;
    }

    if ( connectionMethod != NULL )
    {
        LocalFree(connectionMethod);
        connectionMethod = NULL;
    }

    // It looks as though the system destroys the existing menu handle on its own.
    GetSystemMenu(dlgHwnd, TRUE);
    success = TRUE;

done_out:
    hMenu = NULL;
    dlg = TheNilObj;
    dlgHwnd = NULL;

    return success;
}

/**
 * If the menu is a menu bar attached to a dialog, has the Window redraw the
 * menu bar.
 *
 * @param failOnNoDlg  If not being attached to a dialog is considered an error
 *                     or no.  Usually it is not an error, that's why it is
 *                     *maybe* redraw.  But in some cases, the caller may want
 *                      an error if there is no owener dialog.
 *
 * @return True for no error, false otherwise.
 *
 * @assumes The caller has already reset .SystemErrorCode to 0 and is only
 *          calling this function when not in an error state.  If DrawMenuBar()
 *          fails, this function then sets .SystemErrorCode.  This seems
 *          unlikely.
 */
BOOL CppMenu::maybeRedraw(bool failOnNoDlg)
{
    BOOL success = TRUE;
    if ( dlg != TheNilObj && isMenuBar() )
    {
        success = DrawMenuBar(dlgHwnd);
        if ( ! success )
        {
            oodSetSysErrCode(c->threadContext);
        }
    }
    else if ( failOnNoDlg )
    {
        success = FALSE;
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
    }
    return success;
}


logical_t CppMenu::connectCommandEvent(RexxObjectPtr rxID, CSTRING methodName, RexxObjectPtr dialog)
{
    logical_t success = FALSE;

    pCEventNotification pcen = basicConnectSetup(dialog);
    if ( pcen == NULL )
    {
        goto done_out;
    }

    int32_t id = oodGlobalID(c, rxID, 1, true);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    if ( *methodName == '\0' )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
        goto done_out;
    }

    uint32_t rc;
    if ( isSystemMenu() )
    {
        rc = _connectSysItem(pcen, c, id, methodName);
    }
    else
    {
        rc = _connectItem(pcen, c, id, methodName);
    }
    if ( rc == 0 )
    {
        success = TRUE;
    }
    else
    {
        oodSetSysErrCode(c->threadContext, rc);
    }

done_out:
    return success;
}


logical_t CppMenu::connectAllCommandEvents(CSTRING methodName, RexxObjectPtr dialog)
{
    logical_t success = FALSE;

    pCEventNotification pcen = basicConnectSetup(dialog);
    if ( pcen == NULL )
    {
        goto done_out;
    }

    // We can just pass methodName along.  If it was omitted, it is NULL, and
    // menuConnectItems() handles that correctly.
    uint32_t rc = menuConnectItems(hMenu, pcen, c, methodName, isSystemMenu());
    if ( rc == 0 )
    {
        success = TRUE;
    }
    else
    {
        oodSetSysErrCode(c->threadContext, rc);
    }

done_out:
    return success;
}

logical_t CppMenu::connectSomeCommandEvents(RexxObjectPtr rxItemIds, CSTRING method, logical_t byPosition,
                                            RexxObjectPtr _dlg)
{
    logical_t success = FALSE;
    uint32_t *ids = NULL;
    char *name = NULL;

    pCEventNotification pcen = basicConnectSetup(_dlg);
    if ( pcen == NULL )
    {
        goto done_out;
    }

    if ( ! c->IsOfType(rxItemIds, "COLLECTION") )
    {
        wrongClassException(c->threadContext, 1, "Collection");
        goto done_out;
    }

    // The 'method' argument is optional in the Rexx method, so NULL is okay.
    // But, if the user does supply a method name, it can not be the emptry
    // strin.
    if ( method != NULL && *method == '\0' )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
        goto done_out;
    }

    size_t count;
    ids = getAllIDs(rxItemIds, &count, byPosition);
    if ( ids == NULL )
    {
        goto done_out;
    }

    uint32_t id;
    uint32_t rc;
    char buf[256];
    MENUITEMINFO mii;
    for ( size_t i = 0; i < count; i++ )
    {
        id = ids[i];

        // We don't want to connect the item if it is not a menu command item,
        // so we go ahead and get the menu item text, even though we might not
        // use it.  That gives us access to a filled in mii to use to check the
        // type of the menu item.
        if ( ! getItemText(id, byPosition, buf, sizeof(buf), &mii) )
        {
            goto done_out;
        }

        if ( _isSeparator(&mii) || _isSubMenu(&mii) )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
            goto done_out;
        }

        if ( method == NULL )
        {
            name = strdup_2methodName(buf);
            if ( name == NULL )
            {
                oodSetSysErrCode(c->threadContext, ERROR_NOT_ENOUGH_MEMORY);
                goto done_out;
            }

            // If the programmer used a menu name like "..." we now have the
            // empty string.  This is an error, but it might be hard for the
            // programmer to figure what the problem is.
            if ( *name == '\0' )
            {
                oodSetSysErrCode(c->threadContext, ERROR_INVALID_PARAMETER);
                goto done_out;
            }
        }

        if ( isSystemMenu() )
        {
            rc = _connectSysItem(pcen, c, id, method == NULL ? name : method);
        }
        else
        {
            rc = _connectItem(pcen, c, id, method == NULL ? name : method);
        }

        if ( rc != 0 )
        {
            oodSetSysErrCode(c->threadContext, rc);
            goto done_out;
        }
    }

    success = TRUE;

done_out:
    safeFree(ids);
    safeFree(name);
    return success;
}

#define MENU_MESSAGE_KEYWORDS "INITMENU, INITMENUPOPUP, or CONTEXTMENU"

uint32_t CppMenu::string2WM(CSTRING keyWord)
{
    uint32_t wm = 0;
    if ( stricmp(keyWord, "CONTEXTMENU") == 0 )
    {
        wm = WM_CONTEXTMENU;
    }
    else if ( stricmp(keyWord, "INITMENU") == 0 )
    {
        wm = WM_INITMENU;
    }
    else if ( stricmp(keyWord, "INITMENUPOPUP") == 0 )
    {
        wm = WM_INITMENUPOPUP;
    }
    return wm;
}

logical_t CppMenu::connectMenuMessage(CSTRING methodName, CSTRING keyWord, HWND hFilter, RexxObjectPtr dialog)
{
    logical_t success = FALSE;

    pCEventNotification pcen = basicConnectSetup(dialog);
    if ( pcen == NULL )
    {
        goto done_out;
    }

    uint32_t windowMessage = string2WM(keyWord);
    if ( windowMessage == 0 )
    {
        wrongArgValueException(c->threadContext, 2, MENU_MESSAGE_KEYWORDS, keyWord);
        return FALSE;
    }

    uint32_t tag = TAG_MENU;

    switch ( windowMessage )
    {
        case WM_CONTEXTMENU :
        {
            if ( hFilter != NULL )
            {
                success = addMiscMessage(pcen, c, WM_CONTEXTMENU, UINT32_MAX, (WPARAM)hFilter, UINTPTR_MAX, 0, 0, methodName, tag);
            }
            else
            {
                success = addMiscMessage(pcen, c, WM_CONTEXTMENU, UINT32_MAX, 0, 0, 0, 0, methodName, tag);
            }
        }
            break;

        case WM_INITMENU :
            success = addMiscMessage(pcen, c, WM_INITMENU, UINT32_MAX, (WPARAM)hMenu, UINTPTR_MAX, 0, 0, methodName, tag);
            break;

        case WM_INITMENUPOPUP :
            if ( isMenuBar() )
            {
                if ( hFilter != NULL )
                {
                    // This will trap WM_INITPOPUP messages for the specified submenu.
                    success = addMiscMessage(pcen, c, WM_INITMENUPOPUP, UINT32_MAX, (WPARAM)hFilter, UINTPTR_MAX, 0, 0, methodName, tag);
                }
                else
                {
                    // This will trap all WM_INITPOPU messages sent to the dialog the menubar is attached to.
                    success = addMiscMessage(pcen, c, WM_INITMENUPOPUP, UINT32_MAX, 0, 0, 0, 0, methodName, tag);
                }
            }
            else
            {
                // This will trap only WM_INITPOPUP messages for *this* menu.
                success = addMiscMessage(pcen, c, WM_INITMENUPOPUP, UINT32_MAX, (WPARAM)hMenu, UINTPTR_MAX, 0, 0, methodName, tag);
            }
            break;

        default :
            break;

    }

    if ( ! success )
    {
        oodSetSysErrCode(c->threadContext, ERROR_NOT_ENOUGH_MEMORY);
    }

done_out:
    return success;
}

pCEventNotification CppMenu::basicConnectSetup(RexxObjectPtr dialog)
{
    oodResetSysErrCode(c->threadContext);
    pCEventNotification pcen = NULL;

    if ( hMenu == NULL )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_MENU_HANDLE);
        goto done_out;
    }

    if ( dialog == NULLOBJECT  )
    {
        if ( dlg == TheNilObj )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
            goto done_out;
        }
        dialog = dlg;
    }
    else
    {
        if ( ! c->IsOfType(dialog, "PLAINBASEDIALOG") )
        {
            oodSetSysErrCode(c->threadContext, ERROR_WINDOW_NOT_DIALOG);
            goto done_out;
        }
    }

    pcen = dlgToEventNotificationCSelf(c, dialog);

done_out:
    return pcen;
}

logical_t CppMenu::getItemText(uint32_t id, logical_t byPosition, char *text, uint32_t cch, MENUITEMINFO *mii)
{
    // MIIM_FTYPE and MIIM_SUBMENU are added so the caller can determine the
    // type of the menu item.
    ZeroMemory(mii, sizeof(MENUITEMINFO));
    mii->cbSize = sizeof(MENUITEMINFO);
    mii->fMask = MIIM_STRING | MIIM_FTYPE | MIIM_SUBMENU;
    mii->dwTypeData = (LPSTR)text;
    mii->cch = cch;
    if ( GetMenuItemInfo(hMenu, id, (BOOL)byPosition, mii) == 0 )
    {
        oodSetSysErrCode(c->threadContext);
        return FALSE;
    }
    return TRUE;
}


/**
 * Checks whether there are menu items to be connected to the owner / assigned
 * dialog, and, if so, makes the connections.
 *
 * @return True on success, false if an error happend while making a connection.
 *
 * @note  If an error occurs, then .SystemErrorCode gets set.  If there is
 *        'connectionRequested' then on return the connection queue has been
 *        released, even if there was an error.
 *
 * @assumes  This method is *only* called when there is an owner dialog.
 */
BOOL CppMenu::checkPendingConnections()
{
    BOOL result = FALSE;
    uint32_t rc = 0;
    pCEventNotification pcen = dlgToEventNotificationCSelf(c, dlg);

    if ( connectionRequested )
    {
        MapItem *m;
        for ( size_t i = 0; i < connectionQIndex ; i++ )
        {
            m = connectionQ[i];

            // We could run out of message table entries.
            rc = _connectItem(pcen, c, m->id, m->methodName);
            if ( rc != 0 )
            {
                oodSetSysErrCode(c->threadContext, rc);
                break;
            }
        }

        releaseConnectionQ();
        if ( rc != 0 )
        {
            goto done_out;
        }
    }
    result = checkAutoConnect(pcen);

done_out:
    return result;
}


BOOL CppMenu::checkAutoConnect(pCEventNotification pcen)
{
    if ( autoConnect )
    {
        uint32_t rc = menuConnectItems(hMenu, pcen, c, connectionMethod, isSystemMenu());
        if ( rc != 0 )
        {
            oodSetSysErrCode(c->threadContext, rc);
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * Initially sets, or changes, the autoconnection status.
 *
 * @param on            Set autoconnection on or off.
 * @param methodName    A method name to connect all the items to one method.
 *
 * @return True on success, false on error.  The only error would be a memory
 *         allocation error.
 *
 * @remarks  In order to allow the user to change the current status of
 *           autoconnection, we allow the method name to be the empty string.
 *           When changing the current status, if the user omits the method name
 *           argument, then connectionMethod is left alone.  That is, if there
 *           is already a method name, it is left the same, if it is null it is
 *           left null.
 *
 *           However, if there is already a method name set, but the user wants
 *           to change that to no method name, (which means each menu item is
 *           connected to a unique method based on the text of the item,) that
 *           can be done by passing in the empty string.
 */
bool CppMenu::setAutoConnection(logical_t on, CSTRING methodName)
{
    if ( on )
    {
        autoConnect = true;
        if ( methodName != NULL )
        {
            size_t len = strlen(methodName);

            if ( connectionMethod != NULL  )
            {
                LocalFree(connectionMethod);
                connectionMethod = NULL;
            }

            // If len is 0, we're done.  The existing method name, if it
            // existed, is removed.
            if ( len > 0 )
            {
                connectionMethod = (char *)LocalAlloc(LPTR, len + 1);
                if ( connectionMethod == NULL )
                {
                    return false;
                }
            }
        }
    }
    else
    {
        autoConnect = false;
        if ( connectionMethod != NULL  )
        {
            LocalFree(connectionMethod);
            connectionMethod = NULL;
        }
    }
    return true;
}


/**
 * This is the implemntation for both popMenu_track and popMenu_show.  See the
 * documentation for either one of those native methods.
 *
 * @param location
 * @param opts
 * @param bothButtons
 * @param excludeRect
 * @param doTrack
 *
 * @return RexxObjectPtr
 *
 * @remarks  The TrackPopupEx() function needs to run in the same thread as the
 *           window loop, so we use SendMessage with our own user method.  We
 *           set up a TRACKPOP structure to pass on the parameters that
 *           TrackPopupEx() needs.
 *
 *           NOTE that currently the Rexx event handler method is invoked using
 *           invokeDispatch(), so the event handler *is* running in a different
 *           thread.  If that should change for some reason, then we need to
 *           check here for the thread ID and determine whether or not to use
 *           SendMessage().
 *
 *           TrackPopupEx expects screen coordinates, be sure to document that
 *           and how to use dlg~clientToScreen() to get the right values.  Also
 *           note that MS doc on WM_CONTEXTMENU is wrong.  x value is in low
 *           word, not high word of LPARAM.  TODO FIXME remove this comment and
 *           put into doc.
 *
 */
RexxObjectPtr CppMenu::trackPopup(RexxObjectPtr location, RexxObjectPtr _dlg, CSTRING opts,
                                  logical_t bothButtons, RexxObjectPtr excludeRect, bool doTrack)
{
    oodResetSysErrCode(c->threadContext);
    RexxObjectPtr result = TheNegativeOneObj;

    TRACKPOP tp = {0};
    TPMPARAMS tpm;

    HWND ownerWindow;
    if ( _dlg == NULLOBJECT )
    {
        if ( dlg == TheNilObj )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_FUNCTION);
            goto done_out;
        }
        ownerWindow = dlgHwnd;
    }
    else
    {
        if ( ! c->IsOfType(_dlg, "PLAINBASEDIALOG") )
        {
            oodSetSysErrCode(c->threadContext, ERROR_WINDOW_NOT_DIALOG);
            goto done_out;
        }

        pCPlainBaseDialog pcpbd = dlgToCSelf(c, _dlg);
        ownerWindow = pcpbd->hDlg;
        if ( ownerWindow == NULL )
        {
            oodSetSysErrCode(c->threadContext, ERROR_INVALID_WINDOW_HANDLE);
            goto done_out;
        }

        // TODO revisit - should we now make this passed in dialog the internal
        // assigned dialog?  Depends on what is easier for the user, but I'm not
        // sure what would be easier for the user ???
    }

    POINT *p = (PPOINT)rxGetPoint(c, location, 1);
    if ( p == NULL )
    {
        goto done_out;
    }
    memcpy(&tp.point, p, sizeof(POINT));

    if ( hMenu == NULL )
    {
        oodSetSysErrCode(c->threadContext, ERROR_INVALID_MENU_HANDLE);
        goto done_out;
    }
    tp.hMenu = hMenu;
    tp.hWnd = ownerWindow;

    uint32_t flags = 0;
    if ( opts != NULL )
    {
        flags = getTrackFlags(opts);
    }
    else
    {
        flags = TPM_RIGHTALIGN | TPM_BOTTOMALIGN;
    }
    tp.flags = flags;
    tp.flags |= (bothButtons ? TPM_RIGHTBUTTON : TPM_LEFTBUTTON);

    if ( doTrack )
    {
        tp.flags |= TPM_NONOTIFY | TPM_RETURNCMD;
    }

    if ( excludeRect != NULLOBJECT )
    {
        PRECT exclude = (PRECT)rxGetRect(c, excludeRect, 5);
        if ( exclude == NULL )
        {
            goto done_out;
        }
        tpm.cbSize = sizeof(TPMPARAMS);
        memcpy(&tpm.rcExclude, exclude, sizeof(RECT));
        tp.lptpm = &tpm;
    }


    // If TPM_RETURNCMD is specified, the return is the menu item selected,
    // otherwise the return is true or false for success, or not.  If the return
    // is BOOL and failed, then tp.dwErr has been set with GetLastError().
    uint32_t ret = (uint32_t)SendMessage(tp.hWnd, WM_USER_CONTEXT_MENU, (WPARAM)&tp, 0);
    if ( doTrack )
    {
        result = c->UnsignedInt32(ret);
    }
    else
    {
        if ( ret == TRUE )
        {
            result = TheTrueObj;
        }
        else
        {
            oodSetSysErrCode(c->threadContext, tp.dwErr);
            result = TheFalseObj;
        }
    }

done_out:
    return result;
}


/**
 * Produces a string consisting of a series of keywords that describe the
 * (presumed) menu item.
 *
 * @param type        The fType member of the MENUITEMINFO structure.
 *
 * @param strptr      [in / out] Pointer to a buffer for the returned string.
 *
 * @return No return
 */
void itemType2String(MENUITEMINFO *mii, char *strptr)
{
    UINT type = mii->fState;

    strptr[0] = '\0';

    if ( _isSeparator(mii) )
    {
        strcat(strptr, "SEPARATOR");
    }
    else if ( _isCommandItem(mii) )
    {
        strcat(strptr, "COMMAND");
    }
    else if ( _isSubMenu(mii) )
    {
        strcat(strptr, "SUBMENU");
    }

    if ( type & MFT_MENUBARBREAK)
    {
        strcat(strptr, " MENUBARBREAK");
    }
    if ( type & MFT_MENUBREAK )
    {
        strcat(strptr, " MENUBREAK");
    }
    if ( type & MFT_OWNERDRAW )
    {
        strcat(strptr, " OWNERDRAW");
    }
    if ( type & MFT_RADIOCHECK )
    {
        strcat(strptr, " RADIO");
    }
    if ( type & MFT_RIGHTJUSTIFY )
    {
        strcat(strptr, " RIGHTJUSTIFY");
    }
    if ( type & MFT_RIGHTORDER )
    {
        strcat(strptr, " RIGHTORDER");
    }
    if ( mii->hbmpItem != NULL )
    {
        strcat(strptr, " BITMAP");
    }
    if ( mii->cch != 0 )
    {
        strcat(strptr, " STRING");
    }
}

void itemState2String(UINT state, char *strptr, bool isSep, bool isPop)
{
    strptr[0] = '\0';

    if ( isSep )
    {
        return;
    }

    if ( ! isPop )
    {
        if ( state & MFS_CHECKED )
        {
            strcat(strptr, "CHECKED");
        }
        else
        {
            strcat(strptr, "UNCHECKED");
        }
    }

    /* Grayed and disabled flags are equal. */
    if ( state & MFS_DISABLED )
    {
        strcat(strptr, " DISABLED GRAYED");
    }
    else
    {
        strcat(strptr, " ENABLED");
    }

    if ( state & MFS_DEFAULT )
    {
        strcat(strptr, " DEFAULT");
    }

    if ( state & MFS_HILITE )
    {
        strcat(strptr, " HILITE");
    }
    else
    {
        strcat(strptr, " UNHILITE");
    }
}

RexxObjectPtr itemFlags2String(CppMenu *cMenu, RexxObjectPtr rxItemID, BOOL byPosition, uint32_t idArgPos, uint32_t mask)
{
    RexxObjectPtr result = TheNegativeOneObj;
    MENUITEMINFO mii;

    if ( ! getMII(cMenu, rxItemID, (BOOL)byPosition, idArgPos, NULL, mask, &mii) )
    {
        goto done_out;
    }

    char buf[256];
    if ( mask == MIIM_STATE )
    {
        itemState2String(mii.fState, buf, _isSeparator(&mii), _isSubMenu(&mii));
    }
    else
    {
        itemType2String(&mii, buf);
    }
    result = cMenu->getContext()->CString(buf);

done_out:
    return result;
}


static bool getMII(CppMenu *cMenu, RexxObjectPtr rxItemID, BOOL byPosition, uint32_t idArgPos, uint32_t *pID,
            UINT fMask, MENUITEMINFO *mii)
{
    bool success = false;
    oodResetSysErrCode(cMenu->getThreadContext());

    // We add FTYPE and SUBMENU so the caller can determine the type of the item
    // if needed.
    ZeroMemory(mii, sizeof(MENUITEMINFO));
    mii->cbSize = sizeof(MENUITEMINFO);
    mii->fMask = fMask | MIIM_FTYPE | MIIM_SUBMENU;

    uint32_t itemID = resolveItemID(cMenu->getContext(), rxItemID, byPosition, idArgPos);
    if ( itemID == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    if ( GetMenuItemInfo(cMenu->getHMenu(), itemID, (BOOL)byPosition, mii) == 0 )
    {
        oodSetSysErrCode(cMenu->getThreadContext());
        goto done_out;
    }
    success = true;
    if ( pID != NULL )
    {
        *pID = itemID;
    }

done_out:
    return success;
}


/**
 *  Retrieves the submenu handle of the specified menu item, and optionally the
 *  ID of that submenu and the resolved position ID if byPosition.
 *
 * @param cMenu
 * @param rxItemID
 * @param byPosition
 * @param pID
 * *param pHMenu
 *
 * @return  The valid handld of the submenu on success, or NULL on failure.
 *
 * @remarks  When GetMenuItemInfo() fails with say, 1456 a menu item was not
 *           found, the mii.hSubMenu member is null.  When GetMenuItemInfo()
 *           does not fail and mii.hSubMenu is null, then the item id was a
 *           valid menu item, just not a submenu.  ERROR_INVALID_FUNCTION does
 *           get set, as desired.
 *
 *           There is this one odd behaviour.  If the Rexx programmer has gotten
 *           the handle of a submenu, destroyed the menu, and not removed it
 *           from the parent menu, then queries for the submenu handle again,
 *           GetMenuItemInfo() will return a handle, but the handle will not be
 *           a valid menu item.  This screws up the logic in a number of places,
 *           so when the submenu handle is not null, there is an added check
 *           that the handle is a valid menu handle.
 */
HMENU getSubMenuHandle(CppMenu *cMenu, RexxObjectPtr rxItemID, logical_t byPosition, uint32_t *pID, uint32_t *pPositionID)
{
    oodResetSysErrCode(cMenu->getThreadContext());
    MENUITEMINFO mii = {0};

    uint32_t itemID = resolveItemID(cMenu->getContext(), rxItemID, byPosition, 1);
    if ( itemID == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_SUBMENU | MIIM_ID;

    if ( GetMenuItemInfo(cMenu->getHMenu(), itemID, (BOOL)byPosition, &mii) == 0 )
    {
        oodSetSysErrCode(cMenu->getThreadContext());
        goto done_out;
    }

    if ( mii.hSubMenu == NULL )
    {
        oodSetSysErrCode(cMenu->getThreadContext(), ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    if ( ! IsMenu(mii.hSubMenu) )
    {
        oodSetSysErrCode(cMenu->getThreadContext(), ERROR_INVALID_MENU_HANDLE);
        mii.hSubMenu = NULL;
        goto done_out;
    }

    if ( pID != NULL )
    {
        *pID = mii.wID;
    }
    if ( pPositionID != NULL && byPosition )
    {
        *pPositionID = itemID;
    }

done_out:
    return mii.hSubMenu;
}


/**
 *  Given a Rexx object that could be a numeric by position ID, a numeric
 *  resource ID, or a symbolic ID, resolves it to its unsigned int value, or
 *  raised an exception if it is not valid.
 *
 *  Most of the menu methods that set or modify information about menu items can
 *  use either the resource ID of the menu item, or its numeric position.  This
 *  function determines if the item ID is its item position, and if not,
 *  resolves it symbolic ID, if needed.
 *
 *  Menu item positions are zero-based, we use 1-based for Rexx.
 *
 *  @param rxItemID    The item ID which could be a number or a symbolic ID.
 *
 *  @param byPosition  If true, item ID must be a non-negative number. If false,
 *                     the item ID must be a valid resource ID.
 *
 *  @param argPos      The argument position of rxItemId, used for raised
 *                     exceptions.
 *
 *  @return  The resolved number.
 *
 *  @remarks.  Note in the code below, if ooResolveSymbolicID() returns
 *             OOD_ID_EXCEPTION, the exception has already been raised and it
 *             does not matter what is returned.  So, the return need not be
 *             checked.
 */
static uint32_t resolveItemID(RexxMethodContext *c, RexxObjectPtr rxItemID, logical_t byPosition, size_t argPos)
{
    uint32_t itemID = OOD_ID_EXCEPTION;

    if ( byPosition )
    {
        if ( ! c->UnsignedInt32(rxItemID, &itemID) || itemID == 0 )
        {
            notPositiveException(c->threadContext, argPos, rxItemID);
            goto done_out;
        }
        itemID--;
    }
    else
    {
        itemID = (uint32_t)oodGlobalID(c, rxItemID, argPos, true);

    }

done_out:
    return itemID;
}

/**
 *  Determines if the menu item specified is a separator, a submenu, or a
 *  command item.
 *
 * @param cMenu
 * @param rxItemID
 * @param byPosition
 * @param type
 *
 * @return BOOL
 *
 * @note The MF_STRING flag is misleading in that command items could be a
 *       bitmap only, and submenus could also be a string type.  The flag is
 *       used here internally to signal that we are looking to determine if the
 *       menu item is a command item.
 *
 *   TODO - I believe that the item ID is *always* Rexx arg 1.  If so we could
 *   eliminate a lot of passing around idArgPos.
 */
BOOL isType(CppMenu *cMenu, RexxObjectPtr rxItemID, logical_t byPosition, uint32_t idArgPos, uint32_t type)
{
    BOOL result = FALSE;
    MENUITEMINFO mii;

    // getMII() will have raised an exception for a bad ID and already set
    // .SystemErrorCode for any other error.  If an exception is already raised,
    // we just let it pass through, otherwise we raise our own.
    if ( ! getMII(cMenu, rxItemID, (BOOL)byPosition, idArgPos, NULL, MIIM_FTYPE, &mii) )
    {
        if ( ! cMenu->getContext()->CheckCondition() )
        {
            systemServiceExceptionCode(cMenu->getThreadContext(), API_FAILED_MSG, "GetMenuItemInfo",
                                       oodGetSysErrCode(cMenu->getThreadContext()));
        }
        goto done_out;
    }

    switch ( type )
    {
        case MF_SEPARATOR :
            result = _isSeparator(&mii);
            break;
        case MF_STRING :
            result = _isCommandItem(&mii);
            break;
        default :
            result = _isSubMenu(&mii);
            break;
    }

done_out:
   return result;
}


/**
 * Deletes the specified menu item, if and only if the menu item is the correct
 * type.
 *
 * @param cMenu
 * @param rxItemID
 * @param byPosition
 * @param type
 *
 * @return BOOL
 *
 * @note  The type argument is one of: MF_POPUP, MF_STRING, or MF_SEPARATOR.
 *        Assumes the caller has made sure of that.
 *
 *        Unfortunately, it appears DeleteMenu() is an older API that does not
 *        delete a separator by ID, only by position.  So, if this is a delete
 *        separator by ID, we just do a recursive search for the separator and
 *        delete it by position when it is found.
 */
BOOL deleteMenuItem(CppMenu *cMenu, RexxObjectPtr rxItemID, logical_t byPosition, uint32_t idArgPos, uint32_t type)
{
    BOOL success = FALSE;

    uint32_t itemID;
    MENUITEMINFO mii;

    if ( ! getMII(cMenu, rxItemID, (BOOL)byPosition, idArgPos, &itemID, MIIM_FTYPE, &mii) )
    {
        goto done_out;
    }

    switch ( type )
    {
        case MF_SEPARATOR:
            if ( ! _isSeparator(&mii) )
            {
                oodSetSysErrCode(cMenu->getThreadContext(), ERROR_INVALID_FUNCTION);
                goto done_out;
            }

            if ( ! byPosition )
            {
                uint32_t rc = deleteSeparatorByID(cMenu->getHMenu(), itemID);
                if ( rc == 0 )
                {
                    oodSetSysErrCode(cMenu->getThreadContext(), ERROR_MENU_ITEM_NOT_FOUND);
                }
                else if ( rc == (uint32_t)-1 )
                {
                    success = TRUE;
                    cMenu->maybeRedraw(false);
                }
                else
                {
                    oodSetSysErrCode(cMenu->getThreadContext(), rc);
                }
                goto done_out;
            }
            break;

        case MF_STRING :
            if ( ! _isCommandItem(&mii) )
            {
                oodSetSysErrCode(cMenu->getThreadContext(), ERROR_INVALID_FUNCTION);
                goto done_out;
            }
            break;

        default :
            if ( ! _isSubMenu(&mii) )
            {
                oodSetSysErrCode(cMenu->getThreadContext(), ERROR_INVALID_FUNCTION);
                goto done_out;
            }
            break;
    }

    success = DeleteMenu(cMenu->getHMenu(), itemID, byPositionFlag(byPosition));
    if ( ! success )
    {
        oodSetSysErrCode(cMenu->getThreadContext());
        goto done_out;
    }

    cMenu->maybeRedraw(false);

done_out:
    return success;
}

/**
 * Checks that the state of the specified menu item is the same as the state
 * specified.
 *
 * @param cMenu
 * @param rxItemID
 * @param byPosition
 * @param state
 *
 * @return BOOL
 */
BOOL checkState(CppMenu *cMenu, RexxObjectPtr rxItemID, logical_t byPosition, uint32_t idArgPos, uint32_t state)
{
    BOOL result = FALSE;
    MENUITEMINFO mii;

    if ( ! getMII(cMenu, rxItemID, (BOOL)byPosition, idArgPos, NULL, MIIM_STATE, &mii) )
    {
        if ( ! cMenu->getContext()->CheckCondition() )
        {
            systemServiceExceptionCode(cMenu->getThreadContext(), API_FAILED_MSG, "GetMenuItemInfo",
                                       oodGetSysErrCode(cMenu->getThreadContext()));
        }
        goto done_out;
    }

    result = (mii.fState & state);

done_out:
   return result;
}


/**
 * Given a single item ID, or a collection of item IDs, returns an array
 * containing all the resolved item IDs.  (I.e., RexxObject -> uint32_t)
 *
 *
 * @param rxItemIDs
 * @param items       [in / out]  The number of items in the final array is
 *                    returned here.
 * @param byPosition
 *
 * @return NULL on failure, otherwise a pointer to the array of IDs.  When NULL
 *         is returned, an exception has been raised.
 *
 * @remarks  The itemID arg position in all the native API functions is always
 *           1.
 */
uint32_t *CppMenu::getAllIDs(RexxObjectPtr rxItemIDs, size_t *items, logical_t byPosition)
{
    size_t count = 0;
    uint32_t *result = NULL;
    uint32_t itemID;
    bool isCollection = false;
    RexxArrayObject rxIDs = NULLOBJECT;

    if ( c->IsOfType(rxItemIDs, "COLLECTION") )
    {
        isCollection = true;
        rxIDs = (RexxArrayObject)c->SendMessage0(rxItemIDs, "MAKEARRAY");
        if ( rxIDs == NULLOBJECT )
        {
            // A condition should already be raised, but it could be cryptic to
            // the programmer.  Better to use our own.
            c->ClearCondition();
            doOverException(c->threadContext, rxItemIDs);
            goto done_out;
        }
        count = c->ArrayItems(rxIDs);
        if ( count == 0 )
        {
            emptyArrayException(c->threadContext, 1);
            goto done_out;
        }
    }
    else
    {
        itemID = resolveItemID(c, rxItemIDs, byPosition, 1);
        if ( itemID == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        count = 1;
    }

    result = (uint32_t *)malloc(count * sizeof(uint32_t));
    if ( result == NULL )
    {
        outOfMemoryException(c->threadContext);
        goto done_out;
    }

    *items = count;
    if ( isCollection )
    {
        RexxObjectPtr rxID;

        for ( size_t i = 0; i < count; i++ )
        {
            rxID = c->ArrayAt(rxIDs, i + 1);
            itemID = resolveItemID(c, rxID, byPosition, 1);
            if ( itemID == OOD_ID_EXCEPTION )
            {
                free(result);
                result = NULL;
                goto done_out;
            }
            result[i] = itemID;
        }
    }
    else
    {
        *result = itemID;
    }

done_out:
    return result;
}

/**
 * Sets the state of a menu item, or a collection of menu items, where only a
 * single state flag is changed.  The other state flags, if any, remain
 * unchanged.
 *
 * @param cMenu
 * @param rxItemIDs   A single item ID or a collection of item IDs.
 * @param byPosition
 * @param state       The state flag to change.
 *
 * @return True on success, false on failure.  Non-fatal errors set the
 *         .SystemErrorCode.
 */
BOOL setSingleState(CppMenu *cMenu, RexxObjectPtr rxItemIDs, logical_t byPosition, uint32_t state)
{
    oodResetSysErrCode(cMenu->getThreadContext());
    BOOL success = FALSE;
    MENUITEMINFO mii = {0};
    mii.cbSize = sizeof(MENUITEMINFO);
    size_t count = 0;

    uint32_t *ids = cMenu->getAllIDs(rxItemIDs, &count, byPosition);
    if ( ids == NULL )
    {
        goto done_out;
    }

    uint32_t itemID;

    for ( size_t i = 0; i < count; i++)
    {
        itemID = ids[i];
        // First get the existing state and type
        mii.fMask = MIIM_STATE | MIIM_FTYPE | MIIM_SUBMENU;

        if ( GetMenuItemInfo(cMenu->getHMenu(), itemID, (BOOL)byPosition, &mii) == 0 )
        {
            oodSetSysErrCode(cMenu->getThreadContext());
            goto done_out;
        }
        /*
        if ( _isSeparator(&mii) )
        {
            oodSetSysErrCode(cMenu->getThreadContext(), ERROR_INVALID_FUNCTION);
            goto done_out;
        }
        */
        mii.fMask = MIIM_STATE;

        switch ( state )
        {
            case MFS_ENABLED :
                mii.fState &= ~MFS_DISABLED;
                break;

            case MFS_DISABLED :
                mii.fState |= MFS_DISABLED;
                break;

            case MFS_CHECKED :
                mii.fState |= MFS_CHECKED;
                break;

            case OOD_MFS_UNCHECKED :
                mii.fState &= ~MFS_CHECKED;
                break;

            case MFS_HILITE :
                mii.fState |= MFS_HILITE;
                break;

            case OOD_MFS_UNHILITE :
                mii.fState &= ~MFS_HILITE;
                break;

            default :
                goto done_out;
                break;
        }

        if ( SetMenuItemInfo(cMenu->getHMenu(), itemID, byPositionFlag(byPosition), &mii) == 0 )
        {
            oodSetSysErrCode(cMenu->getThreadContext());
            goto done_out;
        }
    }
    success = TRUE;

done_out:
    safeFree(ids);
    return success;
}


/**
 * Get or Set the help context ID for a menu or submenu.
 *
 * @param hMenu    Handle of menu or submenu.
 * @param helpID   The ID to set, ignored if get.
 * @param recurse  If true, the help ID is also set for all submenus of the
 *                 menu, if false only the specified menu has its help ID set.
 *                 Ignored for get.
 * @param id       [in / out]  The help ID is returned here.  This determines if
 *                 it is a get or set.  If null then it is a set.
 *
 * @return 0 on success, a system error code on failure.
 */
static uint32_t menuHelpID(HMENU hMenu, DWORD helpID, BOOL recurse, uint32_t *id)
{
    MENUINFO mi = {0};

    mi.cbSize = sizeof(MENUINFO);
    mi.fMask = MIM_HELPID;

    uint32_t rc = 0;

    if ( id == NULL )
    {
        mi.dwContextHelpID = helpID;
        if ( recurse )
        {
            mi.fMask |= MIM_APPLYTOSUBMENUS;
        }

        if ( SetMenuInfo(hMenu, &mi) == 0 )
        {
            rc = GetLastError();
        }
    }
    else
    {
        if ( GetMenuInfo(hMenu, &mi) == 0 )
        {
            rc = GetLastError();
        }
        else
        {
            *id = mi.dwContextHelpID;
        }
    }
    return rc;
}


/**
 * Creates a new .PopupMenu object from the supplied information.
 *
 * @param c           The method context we are operating under.
 * @param hPopup      Menu handle of the popup menu.
 * @param rxItemID    Possible menu ID as a Rexx object.
 *
 * @param byPosition  Determines if rxItemID can be used as the menu ID.  If
 *                    true than rxItemID is a position ID and wID must be used
 *                    as the menu ID.
 *
 * @param wID         Menu ID as a non-Rexx object.
 * @param self        The Menu object requesting the creation of the popup
 *                    object, the parent menu of the popup.
 *
 * @return The new PopupMenu object on succes or zero (a Rexx object) on
 *         failure.
 *
 * @assumes  The caller has already done verification on rxItemID and wID; that
 *           it is only our duty to pick which one to use, dependent on the
 *           value of byPosition.
 *
 * @remarks  If hPopup already has a help ID, using 0 in the new() message
 *           allows the help ID to be retained.  It is tempting to think about
 *           querying hPopup for the help ID, and then using that ID in the
 *           new() message.  This is just a waste of time, it just resets the
 *           help ID that the menu already has.
 */
RexxObjectPtr newPopupFromHandle(RexxMethodContext *c, HMENU hPopup, RexxObjectPtr rxItemID,
                                 logical_t byPosition, uint32_t wID, RexxObjectPtr self)
{
    RexxObjectPtr result = TheZeroObj;

    RexxObjectPtr subMenuID = rxItemID;
    if ( byPosition )
    {
        subMenuID = c->UnsignedInt32(wID);
    }

    RexxObjectPtr popupClass = rxGetContextClass(c, "POPUPMENU");
    if ( popupClass == NULLOBJECT )
    {
        // If this happens things are seriously mangled, exception 98.909 has
        // been raised.
        goto done_out;
    }

    RexxArrayObject args = c->ArrayOfThree(subMenuID, TheZeroObj, c->NewPointer(hPopup));
    RexxObjectPtr popupMenu = c->SendMessage(popupClass, "NEW", args);
    if ( popupMenu == NULLOBJECT )
    {
        // This is a good error code for this, but how could it happen to begin
        // with?
        c->ClearCondition();
        oodSetSysErrCode(c->threadContext, OR_INVALID_OID);
        goto done_out;
    }

    result = popupMenu;

done_out:
    return result;
}


/**
 * Gets or sets the dwMenuData member for the specified menu.  dwMenuData is
 * used to store the Rexx object that represents the Windows menu.
 *
 * @param c      The method context we are operating under.
 *
 * @param hMenu  Windows menu handle
 *
 * @param data   [in / out] Pointer to a RexxObjectPtr.
 *
 *               If the RexxObjectPtr is null, then a GetMenuInfo is done and
 *               the current value of the dwMenuData is returned here.  The
 *               stored value could very well be null if nothing has been put
 *               there.
 *
 *               If the RexxObjectPtr is not null, then a SetMenuInfo is done
 *               and the RexxObjectPtr is stored in the dwMenuData member.
 *
 * @return True on success, fails on failure.  On failure, an exception has been
 *         raised.
 *
 * @assumes  hMenu is a valid menu handle.
 *
 * @remarks  We want to avoid raising exceptions if possible.  As long as hMenu
 *           is a valid menu handle, then this should never fail.  If it does,
 *           things are seriously wrong and an exception seems wise.
 */
bool menuData(RexxMethodContext *c, HMENU hMenu, RexxObjectPtr *data)
{
    MENUINFO mi = {0};

    mi.cbSize = sizeof(MENUINFO);
    mi.fMask = MIM_MENUDATA;
    mi.dwMenuData = (ULONG_PTR)*data;

    bool success = true;

    if ( mi.dwMenuData == NULL )
    {
        if ( GetMenuInfo(hMenu, &mi) == 0 )
        {
            systemServiceExceptionCode(c->threadContext, API_FAILED_MSG, "GetMenuInfo");
            success = false;
        }
        else
        {
            *data = (RexxObjectPtr)mi.dwMenuData;
        }
    }
    else
    {
        if ( SetMenuInfo(hMenu, &mi) == 0 )
        {
            systemServiceExceptionCode(c->threadContext, API_FAILED_MSG, "SetMenuInfo");
            success = false;
        }
    }
    return success;
}


/**
 * Recursively connects all menu command items in a menu to a method composed of
 * the text of the command item, or to the single method name specified by msg.
 *
 * @param hMenu      Handle of menu to connect command items.
 * @param pcen       EventNotification CSelf for the dialog.
 * @param msg        Name of the method to connect.  When this is null, each
 *                   item is connected to a method name composed from the text
 *                   of the item.
 * @param isSysMenu  If hMenu is a system menu.
 *
 * @return 0 on success, the system error code on failure.
 */
static uint32_t menuConnectItems(HMENU hMenu, pCEventNotification pcen, RexxMethodContext *c, CSTRING msg, bool isSysMenu)
{
    uint32_t rc = 0;
    int count = GetMenuItemCount(hMenu);

    MENUITEMINFO mii = {0};
    unsigned int miiSize = sizeof(MENUITEMINFO);
    unsigned int miiFMask = MIIM_FTYPE | MIIM_SUBMENU;

    for ( int i = 0; i < count; i++)
    {
        ZeroMemory(&mii, miiSize);
        mii.cbSize = miiSize;
        mii.fMask = miiFMask;

        if ( GetMenuItemInfo(hMenu, i, TRUE, &mii) == 0 )
        {
            return GetLastError();
        }

        if ( _isSubMenu(&mii) )
        {
            rc = menuConnectItems(mii.hSubMenu, pcen, c, msg, isSysMenu);
            if ( rc != 0 )
            {
                return rc;
            }
        }
        else if ( _isCommandItem(&mii) )
        {
            TCHAR buf[256];
            mii.fMask = MIIM_ID | MIIM_STRING | MIIM_FTYPE;
            mii.cch = sizeof(buf);
            mii.dwTypeData = buf;

            if ( GetMenuItemInfo(hMenu, i, TRUE, &mii) == 0 )
            {
                return GetLastError();
            }

            char *pMsg = (char *)msg;
            if ( pMsg == NULL )
            {
                // strdup_2methodName removes any '&' and the trailing ... if any.
                pMsg = strdup_2methodName(mii.dwTypeData);
                if ( ! pMsg )
                {
                    return ERROR_NOT_ENOUGH_MEMORY;
                }
            }

            /* Can not use the empty string for a method name. */
            if ( *pMsg == '\0' )
            {
                return ERROR_INVALID_FUNCTION;
            }

            if ( isSysMenu )
            {
                rc = _connectSysItem(pcen, c, mii.wID, pMsg);
            }
            else
            {
                rc = _connectItem(pcen, c, mii.wID, pMsg);
            }

            if ( pMsg != msg )
            {
                safeFree(pMsg);
            }
            if ( rc != 0 )
            {
                return rc;
            }
        }
    }
    return 0;
}


/**
 * Recursively searches for a separator that matches the specified ID, and
 * removes that separator when found.
 *
 * @param hMenu  Menu to start the recursive search.
 * @param id     ID of separator we are looking for.
 *
 * @return (uint32_t)-1 if found and deleted, 0 if not found and no other
 *         errors, otherwise a system error code if one happenings.
 *
 * @note A return of 0 is essentially ERROR_MENU_ITEM_NOT_FOUND.
 */
static uint32_t deleteSeparatorByID(HMENU hMenu, uint32_t id)
{
    DWORD rc = 0;
    int count = GetMenuItemCount(hMenu);

    MENUITEMINFO mii = {0};
    unsigned int miiSize = sizeof(MENUITEMINFO);
    unsigned int miiFMask = MIIM_FTYPE | MIIM_SUBMENU | MIIM_ID;

    for ( int i = 0; i < count; i++)
    {
        ZeroMemory(&mii, miiSize);
        mii.cbSize = miiSize;
        mii.fMask = miiFMask;

        if ( GetMenuItemInfo(hMenu, i, TRUE, &mii) == 0 )
        {
            return GetLastError();
        }

        if ( _isSubMenu(&mii) )
        {
            rc = deleteSeparatorByID(mii.hSubMenu, id);
            if ( rc != 0 )
            {
                return rc;
            }
        }
        else if ( _isSeparator(&mii) && mii.wID == id )
        {
            if ( DeleteMenu(hMenu, i, MF_BYPOSITION) == 0 )
            {
                return GetLastError();
            }
            return (uint32_t)-1;
        }
    }
    return 0;
}


/**
 *  Methods for the .Menu mixin class.
 */
#define MENU_CLASS       "Menu"


/** Menu::connectCommandEvent() [class]
 *
 * This class method connects a menu command item event with a method in the
 * specified dialog.
 *
 * @param id          The resource ID of the menu item to be connected.
 *
 * @param methodName  The method to connect the menu item to.
 *
 * @param dlg         The dlg to connect the menu item too.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_PARAMETER(87) -> The method name argument can not be the
 *        empty string.
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dialog argument is not a
 *        .PlainBaseDialog, (or subclass of course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 */
RexxMethod4(logical_t, menu_connectCommandEvent_cls, RexxObjectPtr, rxID, CSTRING, methodName,
            RexxObjectPtr, dlg, OSELF, self)
{
    logical_t success = FALSE;
    bool isSystemMenu = isOfClassType(context, self, "SystemMenu");

    pCEventNotification pcen = _getPCEN(context, dlg);
    if ( pcen == NULL )
    {
        goto done_out;
    }

    int32_t id = oodGlobalID(context, rxID, 1, true);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    if ( *methodName == '\0' )
    {
        oodSetSysErrCode(context->threadContext, ERROR_INVALID_PARAMETER);
        goto done_out;
    }

    uint32_t rc;
    if ( isSystemMenu )
    {
        rc = _connectSysItem(pcen, context, id, methodName);
    }
    else
    {
        rc = _connectItem(pcen, context, id, methodName);
    }

    (rc == 0) ? success = TRUE : oodSetSysErrCode(context->threadContext, rc);

done_out:
    return success;
}


/** Menu::menuInit() [private]
 *
 * Sets the menu CSelf.
 *
 * @param  cselfObject  The CSelf object for all menus.
 *
 */
RexxMethod1(RexxObjectPtr, menu_menuInit_pvt, RexxObjectPtr, cselfObject)
{
    context->SetObjectVariable("CSELF", cselfObject);
    return NULLOBJECT;
}


/** Menu::uninit()
 *
 *
 *
 * @return The handle to the menu this object represents.
 */
RexxMethod1(RexxObjectPtr, menu_uninit, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;

#ifdef _DEBUG
#if 0
    printf("In UNINIT() of Menu class cMenu=%p\n", cMenu);
#endif
#endif

    if ( cMenu != NULL )
    {
        cMenu->uninitMenu();
    }
    return NULLOBJECT;
}


/** Menu::hMenu()  [attribute get]
 *
 * Gets the hMenu attribute.
 *
 * @return The handle to the menu this object represents.
 */
RexxMethod1(POINTER, menu_getHMenu, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    return cMenu->getHMenu();
}


/** Menu::wID() [attribute get]
 *
 * The resource ID of this menu
 *
 * @return  The resource ID, which may be -1 if the ID was not assigned when
 *          this menu was created.
 */
RexxMethod1(int32_t, menu_wID, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);
    return cMenu->wID;
}


/** Menu::getMenuHandle()
 *
 * Get the raw menu handle of a popup menu, (a submenu,) of this menu.
 *
 * @param rxItemID    The item ID of the popup.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  Whether the popup is found by ID or by
 *                    position.  The default is true, found by position,
 *
 * @return  The menu handle on success, 0 on failure.
 *
 * @note  Sets .SystemErrorCode on failure.
 *
 *        If the item ID specified is not a submenu then the .SystemErrorCode is
 *        set to:
 *
 *        ERROR_INVALID_PARAMETER (87)
 */
RexxMethod3(POINTER, menu_getMenuHandle, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);
    return getSubMenuHandle(cMenu, rxItemID, byPosition, NULL, NULL);
}


/** Menu::releaseMenuHandle()
 *
 * Releases (frees) the operating system resources used by a menu.  The menu is
 * specified by its raw handle.  To release the operating system resources for
 * an ooDialog Menu object, use the destroy() method.
 *
 * @param  handle  The handle of the menu to release.  The handle can be in
 *                 .Pointer format or pointer string (0xffff1111) format. An
 *                 exception is raised if the handle is null.
 *
 * @return True if the Windows API succeeds, otherwise false.
 *
 * @remarks  Sets .SystemErrorCode.
 */
RexxMethod2(logical_t, menu_releaseMenuHandle, RexxObjectPtr, handle, OSELF, self)
{
    oodResetSysErrCode(context->threadContext);
    RexxMethodContext *c = context;
    HMENU hMenu = NULL;
    logical_t success = FALSE;

    if ( c->IsPointer(handle) )
    {
        hMenu = (HMENU)c->PointerValue((RexxPointerObject)handle);
    }
    else if ( c->IsString(handle) )
    {
        hMenu = (HMENU)string2pointer(c->ObjectToStringValue(handle));
    }

    if ( hMenu == NULL )
    {
        invalidTypeException(context->threadContext, 1, INVALID_MENU_HANDLE_MSG);
    }
    else
    {
        if ( DestroyMenu(hMenu) == 0 )
        {
            oodSetSysErrCode(context->threadContext);
        }
        else
        {
            success = TRUE;
        }
    }
    return success;
}


/** Menu::getCount()
 *
 * Returns the number of items in this menu.
 *
 * @return  The number of items on success, -1 on error.
 *
 * @note  Sets .SystemErrorCode on error.
 */
RexxMethod1(int32_t, menu_getCount, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);

   int count = GetMenuItemCount(cMenu->getHMenu());
   if ( count == -1 )
   {
       oodSetSysErrCode(context->threadContext);
   }
   return count;
}


/** Menu::isValidItemID()
 *
 * Determines if a specific item ID would be valid for this menu.
 *
 * In most of the methods for the Menu classes, if an invalid item ID is
 * specified, the method will fail and .SystemErrorCode is set to the failure
 * reason.  However, in some methods there is no way to indicate that the method
 * failed.  For those methods, a syntax error is raised if the item ID is not
 * valid.
 *
 * The isValidItemID() method can be used by the programmer to check if an item
 * ID will raise a syntax error.  This gives the programmer the option of:
 * setting up a trap handler, checking if a item ID is valid before invoking a
 * method, or just let the syntax error be raised.
 *
 * @param rxItemID    The item ID.
 *
 * @param byPosition  [optional]  Whether the item ID is a by position ID or a
 *                    resource ID.  The default is false, a resource ID, just as
 *                    it is in most methods.
 *
 * @return  True if the item id / by position combination is valid, at this
 *          point in time, false if it is not valid.
 *
 * @note  Will set the .SystemErrorCode if the item ID is not valid. The code
 *        should indicate the reason why the ID is not valid.
 *
 *        If the ID is a symbolic ID that can not be resolved, or a position ID
 *        that is not a valid number, .SystemErrorCode is set to:
 *
 *        ERROR_INVALID_PARAMETER (87)
 */
RexxMethod3(logical_t, menu_isValidItemID, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    BOOL result = FALSE;
    MENUITEMINFO mii = {0};

    uint32_t itemID = resolveItemID(context, rxItemID, byPosition, 1);
    if ( context->CheckCondition() )
    {
        context->ClearCondition();
        oodSetSysErrCode(context->threadContext, ERROR_INVALID_PARAMETER);
        goto done_out;
    }

    // The MIIM_FTYPE mask should be valid to query any item in a menu, so we
    // shouldn't get an error from some invalid combination of flags.
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask =  MIIM_FTYPE;

    result = GetMenuItemInfo(cMenu->getHMenu(), itemID, (BOOL)byPosition, &mii);
    if ( ! result )
    {
        oodSetSysErrCode(context->threadContext);
    }

done_out:
    return result;
}


/** Menu::isValidMenu()
 *
 * Checks if *this* menu is valid.  The menu would not be valid if it has been
 * destroyed, deleted, or for some other operating system reason.
 *
 * @return  True if this menu is valid, otherwise false.
 */
RexxMethod1(logical_t, menu_isValidMenu, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);
    return IsMenu(cMenu->getHMenu());
}

/** Menu::isValidMenuHandle()
 *
 * Checks if the specified handle represents a menu that the operating system
 * says is valid.
 *
 * @param  handle  The handle to check.
 *
 * @return  True if the menu handle is valid, otherwise false.
 *
 * @remarks  The intention of IsMenu() is to check if a handle is a valid menu
 *           handle.  Since, in ooDialog, we are trying to maintain the concept
 *           that handles are opaque objects, we need to accept any object for
 *           handle.  Even though, in ooDialog, all menu handles are returned as
 *           a .Pointer.
 */
RexxMethod1(logical_t, menu_isValidMenuHandle, RexxObjectPtr, handle)
{
    return IsMenu((HMENU)oodObj2pointer(context, handle));
}


/** Menu::isSeparator()
 *
 * Determines if the specified menu item is a separator.
 *
 * @param itemID      The ID of the item to query.  This can be a position ID
 *                    or a resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  Whether itemID is a resource ID or a position
 *                    ID. The default is false, a resource ID.
 *
 * @return  True if the item is is a separator, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error and also raises a syntax error, since
 *        there is no way to indicate failure in the return.
 *
 *        The isValidItemID() can be used to check if the item id is valid prior
 *        to invoking this method or the condition could be trapped.  If the
 *        condition is trapped or if isValidItemID() returns false, the
 *        .SystemErrorCode can then be examined for the reason why the item ID
 *        is not valid.
 */
RexxMethod3(logical_t, menu_isSeparator, RexxObjectPtr, itemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return isType(cMenu, itemID, byPosition, 1, MF_SEPARATOR);
}


/** Menu::isCommandItem()
 *
 * Determines if the specified menu item is a command item.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  True if the item is is a command item, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error and also raises a syntax error, since
 *        there is no way to indicate failure in the return.
 *
 *        The isValidItemID() can be used to check if the item id is valid prior
 *        to invoking this method or the condition could be trapped.  If the
 *        condition is trapped or if isValidItemID() returns false, the
 *        .SystemErrorCode can then be examined for the reason why the item ID
 *        is not valid.
 */
RexxMethod3(logical_t, menu_isCommandItem, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return isType(cMenu, rxItemID, byPosition, 1, MF_STRING);
}


/** Menu::isPopup()
 *
 * Determines if the specified menu item is a submenu.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  True if the item is is a submenu, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error and also raises a syntax error, since
 *        there is no way to indicate failure in the return.
 *
 *        The isValidItemID() can be used to check if the item id is valid prior
 *        to invoking this method or the condition could be trapped.  If the
 *        condition is trapped or if isValidItemID() returns false, the
 *        .SystemErrorCode can then be examined for the reason why the item ID
 *        is not valid.
 */
RexxMethod3(logical_t, menu_isPopup, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return isType(cMenu, rxItemID, byPosition, 1, MF_POPUP);
}


/** Menu::destroy()
 *
 *  Releases the operating system resources for the menu this .Menu represents.
 *
 *  If the menu is a menubar attached to a dialog it is first detached.
 *  Normally you would not destroy a menu attached to a window, the operating
 *  system handles that automatically.  It is only when a menu is not attached
 *  to a window that it needs to be destroyed.  This includes popup menus that
 *  are submenus of the menu, but not popup menus that are 'floating' and
 *  assigned to a window.
 *
 *  @return  True on success, othewise false.  The system error code is set on
 *           failure.
 *
 *  @remarks  Sets .SystemErrorCode.
 *
 *            Need some comment for the doc on what the Rexx programmer can
 *            expect after the menu is destroyed.
 */
RexxMethod1(logical_t, menu_destroy, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->destroy();

}

/** Menu::isEnabled()
 *
 * Determines if the specified menu item is enabled.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  True if the item is enabled, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error and also raises a syntax error, since
 *        there is no way to indicate failure in the return.  The condition
 *        could be trapped and .SystemErrorCode examined for the reason.  (The
 *        same code is also in the syntax error message.)
 */
RexxMethod3(logical_t, menu_isEnabled, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return ! checkState(cMenu, rxItemID, byPosition, 1, MFS_DISABLED);
}


/** Menu::isDisabled()
 *
 * Determines if the specified menu item is disabled.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  True if the item is disabled, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error and also raises a syntax error, since
 *        there is no way to indicate failure in the return.  The condition
 *        could be trapped and .SystemErrorCode examined for the reason.  (The
 *        same code is also in the syntax error message.)
 *
 *        Gray and disabled are equivalent.
 *
 * @remarks  Microsoft now treats grayed and disabled as the same.  Disabled
 *           used to be just disabled, but not grayed.  The user had no clue as
 *           to why the menu item wouldn't work.  You can still set that state
 *           by using an older API.  We could add a forceDisable() method, but I
 *           don't see much use for it.
 */
RexxMethod3(logical_t, menu_isDisabled, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return checkState(cMenu, rxItemID, byPosition, 1, MFS_DISABLED);
}


/** Menu::isChecked()
 *
 * Determines if the specified menu item is checked.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  True if the item is checked, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error and also raises a syntax error, since
 *        there is no way to indicate failure in the return.  The condition
 *        could be trapped and .SystemErrorCode examined for the reason.  (The
 *        same code is also in the syntax error message.)
 */
RexxMethod3(logical_t, menu_isChecked, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return checkState(cMenu, rxItemID, byPosition, 1, MFS_CHECKED);
}


/** Menu::enable()
 *
 * Sets the menu item(s) to enabled.  Separators can not be enabled.
 *
 * @param rxItemIDs   The item ID(s) to set enabled.  This can be a single ID or
 *                    a collection of IDs.  The ID(s) can be position IDs or
 *                    resource IDs, depending on the value of byPosition.
 *                    However, if this is a collection of IDs there must be all
 *                    the same type, all resource IDs or all position IDs.
 *
 * @param byPosition  [optional]  If true, rxItemIDs is a positional ID,
 *                    otherwise it is a resource ID. The default is false, a
 *                    resource ID.
 *
 * @return  True on success, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(logical_t, menu_enable, RexxObjectPtr, rxItemIDs, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return setSingleState(cMenu, rxItemIDs, byPosition, MFS_ENABLED);
}


/** Menu::disable()
 *
 * Disables one or more menu items.  Separators can not be disabled.  Note that
 * disable and gray are the exact same thing.
 *
 * @param rxItemIDs   The item ID(s) to disabled.  This can be a single ID or a
 *                    collection of IDs.  The ID(s) can be position IDs or
 *                    resource IDs, depending on the value of byPosition.
 *                    However, if this is a collection of IDs there must be all
 *                    the same type, all resource IDs or all position IDs.
 *
 * @param byPosition  [optional]  If true, rxItemIDs is a positional ID,
 *                    otherwise it is a resource ID. The default is false, a
 *                    resource ID.
 *
 * @return  True on success, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(logical_t, menu_disable, RexxObjectPtr, rxItemIDs, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return setSingleState(cMenu, rxItemIDs, byPosition, MFS_DISABLED);
}


/** Menu::hilite()
 *
 * Sets a menu item to hilited.  Separators can not be hilited.  A high lighted
 * menu item is drawn as though it were the selected item.
 *
 * There is not much use for this method, in the author's opinion, but it is
 * included for completeness.  In addition, this method will accept a collection
 * of item IDs, as do the enable(), check(), disable() methods, but it makes
 * little sense to hilite more than one menu item at a time.
 *
 * @param rxItemIDs   The item ID(s) to be hilited.  This can be a single ID or
 *                    a collection of IDs.  The ID(s) can be position IDs or
 *                    resource IDs, depending on the value of byPosition.
 *                    However, if this is a collection of IDs there must be all
 *                    the same type, all resource IDs or all position IDs.
 *
 * @param byPosition  [optional]  If true, rxItemIDs is a positional ID,
 *                    otherwise it is a resource ID. The default is false, a
 *                    resource ID.
 *
 * @return  True on success, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(logical_t, menu_hilite, RexxObjectPtr, rxItemIDs, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return setSingleState(cMenu, rxItemIDs, byPosition, MFS_HILITE);
}


/** Menu::unHilite()
 *
 * Removes the [hilite {link}] from one or more menu items.  Separators can not
 * be [hilited {link}] or unHilited.
 *
 * @param rxItemIDs   The item ID(s) to be unHilited.  This can be a single ID
 *                    or a collection of IDs.  The ID(s) can be position IDs or
 *                    resource IDs, depending on the value of byPosition.
 *                    However, if this is a collection of IDs there must be all
 *                    the same type, all resource IDs or all position IDs.
 *
 * @param byPosition  [optional]  If true, rxItemIDs is a positional ID,
 *                    otherwise it is a resource ID. The default is false, a
 *                    resource ID.
 *
 * @return  True on success, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(logical_t, menu_unHilite, RexxObjectPtr, rxItemIDs, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return setSingleState(cMenu, rxItemIDs, byPosition, OOD_MFS_UNHILITE);
}


/** Menu::check()
 *
 * Checks one or more menu items.  Separators can not be checked.
 *
 * @param rxItemIDs   The item ID(s) to be checked.  This can be a single ID or
 *                    a collection of IDs.  The ID(s) can be position IDs or
 *                    resource IDs, depending on the value of byPosition.
 *                    However, if this is a collection of IDs there must be all
 *                    the same type, all resource IDs or all position IDs.
 *
 * @param byPosition  [optional]  If true, rxItemIDs is a positional ID,
 *                    otherwise it is a resource ID. The default is false, a
 *                    resource ID.
 *
 * @return  True on success, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(logical_t, menu_check, RexxObjectPtr, rxItemIDs, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return setSingleState(cMenu, rxItemIDs, byPosition, MFS_CHECKED);
}


/** Menu::unCheck()
 *
 * unChecks one or more menu items.  Separators can not be checked or unChecked.
 *
 * @param rxItemIDs   The item ID(s) to be unChecked.  This can be a single ID
 *                    or a collection of IDs.  The ID(s) can be position IDs or
 *                    resource IDs, depending on the value of byPosition.
 *                    However, if this is a collection of IDs there must be all
 *                    the same type, all resource IDs or all position IDs.
 *
 * @param byPosition  [optional]  If true, rxItemIDs is a positional ID,
 *                    otherwise it is a resource ID. The default is false, a
 *                    resource ID.
 *
 * @return  True on success, otherwise false.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(logical_t, menu_unCheck, RexxObjectPtr, rxItemIDs, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return setSingleState(cMenu, rxItemIDs, byPosition, OOD_MFS_UNCHECKED);
}


/** Menu::checkRadio()
 *
 * Checks a specified menu item and makes it a radio item. At the same time, the
 * function clears all other menu items in the associated group and clears the
 * radio-item type flag for those items.
 *
 * The selected item is displayed using a bullet bitmap instead of a check-mark
 * bitmap.
 *
 * @param start       The ID of the first item in the group.
 * @param end         The ID of the last item in the group.
 * @param check       The ID of the item to check.
 * @param byPosition  [optional]  Whether the IDs are position IDs or resource
 *                    IDs. The default is resource IDs.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 */
RexxMethod5(logical_t, menu_checkRadio, RexxObjectPtr, start, RexxObjectPtr, end, RexxObjectPtr, check,
            OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    logical_t success = FALSE;

    int idStart = resolveItemID(context, start, byPosition, 1);
    if ( idStart == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }
    int idEnd = resolveItemID(context, end, byPosition, 2);
    if ( idEnd == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }
    int idCheck = resolveItemID(context, check, byPosition, 3);
    if ( idCheck == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    success = CheckMenuRadioItem(cMenu->getHMenu(), idStart, idEnd, idCheck, byPositionFlag(byPosition));
    if ( ! success )
    {
        oodSetSysErrCode(context->threadContext);
    }

done_out:
    return success;
}


/** Menu::insertSeparator()
 *
 * Inserts a separator into this menu at the specified position.
 *
 * @param before      Specifies where the separator is inserted. The separator
 *                    is inserted before this item. If the menu does not have
 *                    any items yet, then any ID, or 0, can be used.
 *
 * @param rxID        The ID for the separator.
 *
 * @param byPosition  [optional]  Whether the before ID is a position ID or a
 *                    resource ID. The default is false, a resource ID.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        It may seem too much to require that separators have resource IDs, but
 *        in the long run being able to work with menus and not have to use by
 *        position is so much easier that it makes sense to require it.  Simply
 *        use 0 for the second argument if you are determined to make things
 *        hard for yourself.
 */
RexxMethod4(logical_t, menu_insertSeparator, RexxObjectPtr, before, RexxObjectPtr, rxID,
            OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    logical_t success = FALSE;
    MENUITEMINFO mii = {0};

    uint32_t idBefore = resolveItemID(context, before, byPosition, 1);
    if ( idBefore == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    int32_t id =  oodGlobalID(context, rxID, 2, false);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_FTYPE;
    mii.fType = MFT_SEPARATOR;

    if ( id != 0)
    {
        mii.fMask |= MIIM_ID;
        mii.wID = id;
    }

    success = InsertMenuItem(cMenu->getHMenu(), idBefore, byPositionFlag(byPosition), &mii);
    if ( ! success )
    {
        oodSetSysErrCode(context->threadContext);
        goto done_out;
    }

    cMenu->maybeRedraw(false);

done_out:
    return success;
}


/** Menu::insertItem()
 *
 * Inserts a new menu command item into this menu.
 *
 * @param rxBefore    The menu command item is inserted before this menu item.
 * @param rxID        The resource ID for the menu command item.
 * @param text        The text (label) for the menu command item.
 * @param stateOpts   [optional]  String containing keywords to specify the menu
 *                    command item state.
 * @param typeOpts    [optional]  String containing keywords to specify the menu
 *                    command item type.
 * @param byPosition  [optional]  Whether the before ID is a position ID or a
 *                    resource ID. The default is false, a resource ID.
 *
 * @param connect     [opitional] Whether to automatically connect the command
 *                    item to a method in the owning dialog.  This is a <link
 *                    linkend="miConnections">connection request</link> type of
 *                    connection type.  Do not mix this with <link
 *                    linkend="miConnections">autoconnection</link>. The default
 *                    is false.
 *
 * @param methodName  [optional]  If connect is true, then this is the name of
 *                    the method to connect this menu commanditem to.  If this
 *                    argument is omitted and connect is true then the method
 *                    name is automatically constructed from the text for the
 *                    menu item. If connect is false or omitted, this argument
 *                    is ignored.  If connect is true and this argument is used
 *                    it can not be the empty string.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_PARAMETER (87) -> When used, the methodName argument can
 *        not be the empty string.
 *
 * @note  State keywords: DEFAULT NOTDEFAULT DISABLED GRAYED ENABLED UNCHECKED
 *        CHECKED HILITE UNHILITE.
 *
 *        By default the state will be set to: ENABLED UNHILITE NOTDEFAULT
 *        UNCHECKED.
 *
 *        Disabled and grayed are the same thing.
 *
 *        High lighted is the appearance drawn when the mouse is over an item,
 *        or the keyboard is used to move to the item.  When the programmer sets
 *        an item to HILITE, the item will remain high lighted until the mouse
 *        comes over it and is then removed, or the keyboard is used to go to
 *        the item and then move away from it.
 *
 *        A default item is drawn with a bolded appearance.
 *
 * @note  Type keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY RADIO.  By default
 *        no special type is set.
 *
 *        RightJustify - Right-justifies the menu item and any subsequent items.
 *        This value is valid only if the menu item is in a menu bar.  It has no
 *        effect if the item is in a popup menu.
 *
 *        Radio - Displays checked menu items using a radio-button mark instead
 *        of a check mark.
 *
 *        MenuBarBreak - Places the menu item on a new line (for a menu bar) or
 *        in a new column (for a drop-down menu, submenu, or shortcut menu). For
 *        a drop-down menu, submenu, or shortcut menu, a vertical line separates
 *        the new column from the old.
 *
 *        MenuBreak - This is exactly the same as MenuBarBreak, except that no
 *        vertical line is drawn between the columns when the item is not in a
 *        menu bar.
 *
 * @remarks  This function forces the new menu item to have a label and does not
 *           allow for it to have a bitmap.  Menu items are allowed to use
 *           bitmaps instead of (along with?) text.  A future enhancement could
 *           add support for that.
 *
 *           Note to myself: I did check and verify the HILITE behaviour
 */
RexxMethod9(logical_t, menu_insertItem, RexxObjectPtr, rxBefore, RexxObjectPtr, rxID, CSTRING, text,
            OPTIONAL_CSTRING, stateOpts, OPTIONAL_CSTRING, typeOpts, OPTIONAL_logical_t, byPosition,
            OPTIONAL_logical_t, connect, OPTIONAL_CSTRING, methodName, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    BOOL success = FALSE;
    MENUITEMINFO mii = {0};

    uint32_t idBefore = resolveItemID(context, rxBefore, byPosition, 1);
    if ( idBefore == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }
    int32_t id = oodGlobalID(context, rxID, 2, true);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID;
    mii.fType = MFT_STRING;
    mii.dwTypeData = (LPSTR)text;
    mii.wID = id;

    if ( argumentExists(4) )
    {
        mii.fState = getItemStateOpts(stateOpts, 0);
        mii.fMask |= MIIM_STATE;
    }

    if ( argumentExists(5) )
    {
        mii.fType |= getItemTypeOpts(typeOpts, 0);
        mii.fMask |= MIIM_FTYPE;
    }

    if ( ! InsertMenuItem(cMenu->getHMenu(), idBefore, (BOOL)byPosition, &mii) )
    {
        oodSetSysErrCode(context->threadContext);
        goto done_out;
    }

    success = TRUE;

    cMenu->maybeRedraw(false);

    if ( cMenu->isMenuBar() && ! byPosition )
    {
        success = cMenu->maybeConnectItem(id, text, connect, methodName);
    }

done_out:
    return success;
}


/** Menu::insertPopup()
 *
 * Inserts a new popup menu into this menu.
 *
 * @param rxBefore    The popup is inserted before this menu item.
 * @param rxID        The resource ID for the popup menu.
 * @param popup       The popup menu being inserted.
 * @param text        The text (label) for the popup menu.
 * @param stateOpts   [optional]  String containing keywords to specify the popup menu
 *                    state.
 * @param typeOpts    [optional]  String containing keywords to specify the
 *                    popup menu type.
 * @param byPosition  [optional]  Whether the before ID is a position ID or a
 *                    resource ID. The default is false, a resource ID.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 * @note  State keywords: DEFAULT NOTDEFAULT DISABLED GRAYED ENABLED HILITE
 *        UNHILITE.  By default the state will be ENABLED UNHILITE NOTDEFAULT
 *
 *        High lighted is the appearance drawn when the mouse is over an item,
 *        or the keyboard is used to move to the item.  When the programmer sets
 *        an item to HILITE, the item will remain high lighted until the mouse
 *        comes over it and is then removed, or the keyboard is used to go to
 *        the item and then move away from it.
 *
 *        A default item is drawn with a bolded appearance.
 *
 * @note  Type keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY RIGHTORDER
 *
 *        RightJustify - Right-justifies the popup menu and any subsequent
 *        items. This value is valid only if the popup menu is in a menu bar. It
 *        has no effect if the popup menu is in a submenu.
 *
 *        MenuBarBreak - Places the popup menu on a new line (for a menu bar) or
 *        in a new column (for a drop-down menu, submenu, or shortcut menu). For
 *        a drop-down menu, submenu, or shortcut menu, a vertical line separates
 *        the new column from the old.
 *
 *        MenuBreak - This is exactly the same as MenuBarBreak, except that no
 *        vertical line is drawn between the columns when the popup menu is not
 *        in a menu bar.
 *
 *        RightOrder - Specifies that menus cascade right-to-left (the default
 *        is left-to-right). This is used to support right-to-left languages,
 *        such as Arabic and Hebrew.
 *
 * @remarks  This function forces the new menu item to have a label and does not
 *           allow for it to have a bitmap.  Menu items and I think submenus are
 *           allowed to use bitmaps instead of (along with?) text.  A future
 *           enhancement could add support for that.
 *
 *           Note to myself, I have tested this several times, setting a
 *           submenu's state to checked does nothing. I've tested all other
 *           keywords, MENUBARBREAK, MENUBREAK, RIGHTJUSTIFY, RIGHTORDER,
 *           DEFAULT, DISABLED, GRAYED, and HILITE all work.
 */
RexxMethod8(logical_t, menu_insertPopup, RexxObjectPtr, rxBefore, RexxObjectPtr, rxID, RexxObjectPtr, popup, CSTRING, text,
            OPTIONAL_CSTRING, stateOpts, OPTIONAL_CSTRING, typeOpts, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    RexxMethodContext *c = context;
    RexxObjectPtr self = cMenu->getSelf();

    oodResetSysErrCode(context->threadContext);
    logical_t success = FALSE;
    MENUITEMINFO mii = {0};

    uint32_t idBefore = resolveItemID(context, rxBefore, byPosition, 1);
    if ( idBefore == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }
    if ( ! c->IsOfType(popup, "POPUPMENU") )
    {
        wrongClassException(context->threadContext, 3, "PopupMenu");
        goto done_out;
    }
    int32_t id = oodGlobalID(context, rxID, 2, true);
    if ( id == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    CppMenu *cPopMenu = (CppMenu *)c->ObjectToCSelf(popup);

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_ID;
    mii.fType = MFT_STRING;
    mii.hSubMenu = cPopMenu->getHMenu();
    mii.dwTypeData = (LPSTR)text;
    mii.wID = id;

    if ( argumentExists(5) )
    {
        mii.fState = getPopupStateOpts(stateOpts, 0);
        mii.fMask |= MIIM_STATE;
    }

    if ( argumentExists(6) )
    {
        mii.fType |= getPopupTypeOpts(typeOpts, 0);
        mii.fMask |= MIIM_FTYPE;
    }

    success = InsertMenuItem(cMenu->getHMenu(), idBefore, (BOOL)byPosition, &mii);
    if ( ! success )
    {
        oodSetSysErrCode(context->threadContext);
        goto done_out;
    }

    // TODO Now would be the time to add the popup to the self menu submenu bag (?)

    cMenu->maybeRedraw(false);

done_out:
    return success;
}

/** Menu::removeSeparator()
 *
 * Removes (deletes) the specified menu separator from the menu.
 *
 * @param rxItemID    The item id of the separator to remove.
 *
 * @param byPosition  [optional]  Whether the separator to be removed is found
 *                    by ID or by position.  The default is false, found by
 *                    resource ID.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is not a
 *        separator, then the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1)
 */
RexxMethod3(logical_t, menu_removeSeparator, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return deleteMenuItem(cMenu, rxItemID, byPosition, 1, MF_SEPARATOR);
}


/** Menu::removeItem()
 *
 * Removes (deletes) the specified menu command item from the menu.
 *
 * @param rxItemID    The item id of the command item to remove.
 *
 * @param byPosition  [optional]  Whether the command item to be removed is
 *                    found by ID or by position.  The default is false, found
 *                    by ID.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is not a
 *        menu command item, then the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1)
 */
RexxMethod3(logical_t, menu_removeItem, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return deleteMenuItem(cMenu, rxItemID, byPosition, 1, MF_STRING);
}


/** Menu::removePopup()
 *
 * Removes, but does not delete, the specified popup menu from the menu.  See
 * deleteMenu() for a discussion of the distinction between removing and
 * deleting a popup menu.
 *
 * @param rxItemID    The item id of the popup menu to remove.
 *
 * @param byPosition  [optional]  Whether the popup menu to be removed is found
 *                    by ID or by position.  The default is false, found by ID.
 *
 * @return  The removed .PopupMenu object on success, or 0 on failure.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is not a
 *        popup menu, then the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1)
 */
RexxMethod3(RexxObjectPtr, menu_removePopup, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    RexxMethodContext *c = context;
    RexxObjectPtr result = TheZeroObj;

    uint32_t wID = 0;
    uint32_t positionID = 0;

    HMENU hSubMenu = getSubMenuHandle(cMenu, rxItemID, byPosition, &wID, &positionID);
    if ( hSubMenu == NULL )
    {
        goto done_out;
    }

    if ( RemoveMenu(cMenu->getHMenu(), (byPosition ? positionID : wID), byPositionFlag(byPosition)) == 0 )
    {
        oodSetSysErrCode(context->threadContext);
        goto done_out;
    }

    // TODO Check if .PopupMenu object is already stored in the submenu user
    // data and use if so just return it directly.  Otherwise, create it and
    // store it back in the menu user data so it doesn't need to be created
    // again.  ALSO if we are saving PopupMenu objects somewhere, need to save
    // it after creating it.

    RexxObjectPtr menuObj = NULL;
    if ( ! menuData(context, hSubMenu, &menuObj) )
    {
        goto done_out;
    }

    if ( menuObj != NULL )
    {
        result = menuObj;
    }
    else
    {
        // TODO if this fails, we should re-insert the removed menu.  But, will
        // be hard to do if not byPosition.  ?
        result = newPopupFromHandle(context, hSubMenu, rxItemID, byPosition, wID, cMenu->getSelf());

        // TODO if good result store it in dwMenuData.  Not doing this yet.
        /*
        if ( result != TheZeroObj )
        {
            if ( ! menuData(context, hSubMenu, &result)  )
            {
                result = TheZeroObj;
                goto done_out;
            }
        }
        */
    }

    cMenu->maybeRedraw(false);

done_out:
    return result;
}


/** Menu::deletePopup()
 *
 * Deletes the specified popup menu from the menu.
 *
 * When a popup menu (a submenu) is deleted, the operating system frees all
 * resources used by the popup menu and any references to the popup are no
 * longer valid.  To be explicit, this means that any .PopupMenu objects that
 * represent this submenu are no longer valid, which would also include any menu
 * that has this submenu as one of its submenus.
 *
 * To remove a submenu from a menu and still use the submenu, use the
 * removePopup() method.
 *
 * @param rxItemID    The item id of the submenu to remove.
 *
 * @param byPosition  [optional]  Whether the popup menu to be deleted is found
 *                    by ID or by position.  The default is false, found by ID.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.  If the specified menu item is not a
 *        popup menu, then the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1)
 */
RexxMethod3(logical_t, menu_deletePopup, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return deleteMenuItem(cMenu, rxItemID, byPosition, 1, MF_POPUP);
}


/** Menu::getPopup()
 *
 * Gets the specified popup menu from the menu.  The menu is unchanged.
 *
 * @param rxItemID    The item id of the popup menu.
 *
 * @param byPosition  [optional]  Whether the popup menu is found by ID or by
 *                    position.  The default is false, found by ID.
 *
 * @return  A .PopupMenu object on success, or 0 on failure.
 *
 * @note Sets .SystemErrorCode on failure.  In addition to error codes set the
 *       OS, the following error codes may also be set.
 *
 *       If the specified menu item is not a popup menu, then the system error
 *       code is set to:
 *
 *       ERROR_INVALID_FUNCTION (1)
 *
 *       If there is an internal error (getting the .PopupMenu object) the
 *       system error code is set to:
 *
 *       OR_INVALID_OID (1911) The object specified was not found.
 *
 * @remarks  Note below that, if the call to getSubMenuHandle() succeeds,
 *           rxItemID is valid. wID is the correct submenu id, if it was
 *           assigned, or the correct default value if it was not assigned.
 *
 *           So, in all cases wID is the correct ID to use to create a new
 *           PopupMenu object, if needed.  If byPosition, then of course
 *           rxItemID is not the menu's resource ID.  If not byPosition, then
 *           rxItemID equals wID, but is already a Rexx object.
 */
RexxMethod3(RexxObjectPtr, menu_getPopup, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    RexxObjectPtr result = TheZeroObj;

    uint32_t wID = 0;

    HMENU hSubMenu = getSubMenuHandle(cMenu, rxItemID, byPosition, &wID, NULL);
    if ( hSubMenu == NULL )
    {
        goto done_out;
    }

    // TODO We are not yet storing the Rexx object, so this will always come
    // back NULL. ALSO if we are saving PopupMenu objects somewhere, need to
    // save it after creating it.

    RexxObjectPtr menuObj = NULL;
    if ( ! menuData(context, hSubMenu, &menuObj) )
    {
        goto done_out;
    }

    if ( menuObj != NULL )
    {
        result = menuObj;
    }
    else
    {
        result = newPopupFromHandle(context, hSubMenu, rxItemID, byPosition, wID, cMenu->getSelf());

        // TODO if good result store it in dwMenuData.  Not doing this yet.
        /*
        if ( result != TheZeroObj )
        {
            if ( ! menuData(context, hSubMenu, &result)  )
            {
                result = TheZeroObj;
            }
        }
        */
    }

done_out:
    return result;
}


/** Menu::getItemState()
 *
 * Returns a string of 0 or more keywords that indicate the current state of the
 * specified menu item.  The state for a separator will be the emtpy string.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  The keyword string on success, and -1 on failure.
 *
 * @note  Sets the .SystemErrorCode on failure.
 *
 *        The set of possible keywords is:
 *
 *        CHECKED UNCHECKED DISABLED GRAYED ENABLED DEFAULT HILITE UNHILITE
 *
 * @remarks  Note to myself, if separators are allowed, the string comes back:
 *           UNCHECKED DISABLED GRAYED UNHILITE, which makes sense.
 */
RexxMethod3(RexxObjectPtr, menu_getItemState, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return itemFlags2String(cMenu, rxItemID, (BOOL)byPosition, 1, MIIM_STATE);
}


/** Menu::getType()
 *
 * Returns a string of 0 or more keywords that indicate the type of the
 * specified menu item.
 *
 * @param rxItemID    The item ID to query.  This can be a position ID or a
 *                    resource ID, depending on the value of byPosition.
 *
 * @param byPosition  [optional]  If true, rxItemID is a positional ID,
 *                    otherwise it is a resource ID. The default is false.
 *
 * @return  The keyword string on success, and -1 on failure.
 *
 * @note  Sets the .SystemErrorCode on failure.
 *
 *        The set of possible keywords is:
 *
 *        SEPARATOR COMMAND SUBMENU MENUBARBREAK MENUBREAK OWNERDRAW RADIO
 *        RIGHTJUSTIFY RIGHTORDER BITMAP STRING
 *
 */
RexxMethod3(RexxObjectPtr, menu_getItemType, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return itemFlags2String(cMenu, rxItemID, (BOOL)byPosition, 1, MIIM_FTYPE | MIIM_SUBMENU | MIIM_BITMAP | MIIM_STRING);
}


/** Menu::getID()
 *
 * Gets the resource ID of the specified menu item.  This is always done by
 * position, if the ID is already known, there is no use querying for it.
 *
 * @param   pos  The position of the menu item.
 *
 * @return  The id of the menu item on success, which is always positive, -1 on
 *          failure.
 *
 * @note  Sets .SystemErrorCode,
 *
 *        Remember that when using by position you must use the menu that
 *        actually holds the menu item, the item can not be in a submenu of the
 *        menu.
 */
RexxMethod2(RexxObjectPtr, menu_getID, uint32_t, pos, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    RexxObjectPtr result = TheNegativeOneObj;
    MENUITEMINFO mii = {0};

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID;

    if ( GetMenuItemInfo(cMenu->getHMenu(), pos, TRUE, &mii) == 0 )
    {
        oodSetSysErrCode(context->threadContext);
    }
    else
    {
        result = context->UnsignedInt32(mii.wID);
    }
    return result;
}


/** Menu::setID()
 *
 * Sets the resource ID of the specified menu item.  This would usually be done
 * by position, to maybe set the ID of an item that does not have an ID.
 * However, the method could also be used to change the existing ID of an item.
 * For this reason, by resource ID is supported.
 *
 * @param   itemID      Specifies the menu item whose resource ID is being set.
 * @param   newID       The new resource ID.
 * @param   byPosition  [optional]  Whether itemID is a resource ID or a
 *                      position ID. The default is true, a position ID, because
 *                      this method would most often be used with a by position
 *                      id.
 *
 * @return  True on success, false on failure.
 *
 * @note  Sets .SystemErrorCode.
 *
 *        Remember that when using by position you must use the menu that
 *        actually holds the menu item, the item can not be in a submenu of the
 *        menu.
 */
RexxMethod4(logical_t, menu_setID, RexxObjectPtr, rxItemID, RexxObjectPtr, newID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    logical_t success = FALSE;
    MENUITEMINFO mii = {0};

    uint32_t itemID = resolveItemID(context, rxItemID, byPosition, 1);
    if ( itemID == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }
    mii.wID = oodGlobalID(context, newID, 2, true);
    if ( mii.wID == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID;
    if ( SetMenuItemInfo(cMenu->getHMenu(), itemID, byPositionFlag(byPosition), &mii) == 0 )
    {
        oodSetSysErrCode(context->threadContext);
        goto done_out;
    }
    success = TRUE;

done_out:
    return success;
}


/** Menu::setHelpID()
 *
 * Sets the Help context identifier for the this menu, and optionally all
 * submenus. Note that unlike most resource IDs, the help ID can be a full 32
 * bits instead of 16 bits.
 *
 * @param  id       The ID to be set.
 * @param  recurse  If true, set this menu and all submenus.  The default is
 *                  false.
 *
 * @return True on success, false on failure.
 *
 * @note  Sets .SystemErrorCode on failure.
 *
 *  @remarks  We don't check for a null menu handle, the OS will check for us
 *            and set last error.
 */
RexxMethod3(logical_t, menu_setHelpID, RexxObjectPtr, id, OPTIONAL_logical_t, recurse, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    logical_t result = FALSE;

    DWORD helpID = oodGlobalID(context, id, 1, true);
    if ( helpID == OOD_ID_EXCEPTION )
    {
        goto done_out;
    }

    uint32_t rc = menuHelpID(cMenu->getHMenu(), helpID, (BOOL)recurse, NULL);
    if ( rc == 0 )
    {
        result = TRUE;
    }
    else
    {
        oodSetSysErrCode(context->threadContext, rc);
    }

done_out:
    return result;
}


/** Menu::getHelpID()
 *
 *  Gets the Help context identifier for the menu.
 *
 *  @return On success the help ID, or 0 if the help ID for the menu is not set.
 *          On failure, -1.
 *
 *  @note  Sets .SystemErrorCode on failure.
 *
 *  @remarks  We don't check for a null menu handle, the OS will check for us
 *            and set last error.
 */
RexxMethod1(RexxObjectPtr, menu_getHelpID, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);

    RexxObjectPtr result = TheNegativeOneObj;
    uint32_t helpID;

    uint32_t rc = menuHelpID(cMenu->getHMenu(), 0, 0, &helpID);
    if ( rc == 0 )
    {
        result = context->UnsignedInt32(helpID);
    }
    else
    {
        oodSetSysErrCode(context->threadContext, rc);
    }
    return result;
}


/** Menu::setMaxHeight()
 *
 * Sets the maximum height for this menu, and optionally all submenus.
 *
 * When a menu reaches the maximum height, scroll bars are automatically added.
 * The default height is the height of the screen.  To revert back to the
 * defualt height use 0.
 *
 * @param  height   The maximum height, in pixels, for the menu.
 * @param  recurse  If true, set the height for this menu and all submenus.  The
 *                  default is false.
 *
 * @return True on success, false on failure.
 *
 * @note  Sets .SystemErrorCode on failure.
 */
RexxMethod3(logical_t, menu_setMaxHeight, uint32_t, height, OPTIONAL_logical_t, recurse, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->setMaxHeight(height, recurse);
}


/** Menu::getMaxHeight()
 *
 *  Gets the current maximum height for the menu in pixels.  Note that 0,
 *  indicates the height of the screen.
 *
 *  @return On success the maximum height for the menu. On failure, -1.
 *
 *  @note  Sets .SystemErrorCode on failure.
 */
RexxMethod1(int, menu_getMaxHeight, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->getMaxHeight();
}


/** Menu::setText()
 *
 * Sets the text of the specified menu item.  Separator menu items do not have
 * text
 *
 * @param   itemID      Specifies the menu item whose text is being set.
 * @param   text        The text for the item
 * @param   byPosition  [optional]  Whether itemID is a resource ID or a
 *                      position ID. The default is false, a resource ID.
 *
 * @return  True on success, false on failure.
 *
 * @note  Sets .SystemErrorCode.  If the specified menu item is a separator, the
 *        system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod4(logical_t, menu_setText, RexxObjectPtr, rxItemID, CSTRING, text, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    logical_t success = FALSE;

    MENUITEMINFO mii;
    uint32_t itemID;

    if ( ! getMII(cMenu, rxItemID, (BOOL)byPosition, 1, &itemID, MIIM_FTYPE, &mii) )
    {
        goto done_out;
    }
    if ( _isSeparator(&mii) )
    {
        oodSetSysErrCode(context->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    ZeroMemory(&mii, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING;
    mii.dwTypeData = (LPSTR)text;
    if ( SetMenuItemInfo(cMenu->getHMenu(), itemID, (BOOL)byPosition, &mii) == 0 )
    {
        oodSetSysErrCode(context->threadContext);
        goto done_out;
    }
    success = TRUE;

done_out:
    return success;
}


/** Menu::getText()
 *
 * Gets the text of the specified menu item.  Separator menu items do not have
 * text.
 *
 * @param   itemID      Specifies the menu item whose text is being set.
 * @param   byPosition  [optional]  Whether itemID is a resource ID or a
 *                      position ID. The default is false, a resource ID.
 *
 * @return  The text of the specified menu item on success, the empty string on
 *          failure.
 *
 * @note  Sets .SystemErrorCode on failure.  If the specified menu item is a
 *        separator, the system error code is set to:
 *
 *        ERROR_INVALID_FUNCTION (1) -> Incorrect function.
 */
RexxMethod3(RexxStringObject, menu_getText, RexxObjectPtr, rxItemID, OPTIONAL_logical_t, byPosition, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    char buf[256];
    *buf = '\0';

    MENUITEMINFO mii = {0};
    uint32_t itemID;

    if ( ! getMII(cMenu, rxItemID, (BOOL)byPosition, 1, &itemID, MIIM_FTYPE, &mii) )
    {
        goto done_out;
    }
    if ( _isSeparator(&mii) )
    {
        oodSetSysErrCode(context->threadContext, ERROR_INVALID_FUNCTION);
        goto done_out;
    }

    ZeroMemory(&mii, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING;
    mii.dwTypeData = (LPSTR)buf;
    mii.cch = sizeof(buf);
    if ( GetMenuItemInfo(cMenu->getHMenu(), itemID, (BOOL)byPosition, &mii) == 0 )
    {
        oodSetSysErrCode(context->threadContext);
    }

done_out:
    return context->CString(buf);
}


/** Menu::setAutoConnection()
 *
 *  Turns auto connect on or off, or changes the way the method name to auto
 *  connect is determined.  [do link to auto connection section]
 *
 *  @param  on          If true, auto connection is turned on, if false it is
 *                      turned off.
 *
 *  @param  methodName  [optional]  The method name to connect all menu items
 *                      to.  If this argument is omitted, each menu item is
 *                      connected to a method whose name is constructed from the
 *                      text of the item.  If auto connection is off, this
 *                      argument is ignored.
 *
 *  @return  This method does not return a value.
 */
RexxMethod3(RexxObjectPtr, menu_setAutoConnection, logical_t, on, OPTIONAL_CSTRING, methodName, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    if ( ! cMenu->setAutoConnection(on, methodName) )
    {
        outOfMemoryException(context->threadContext);
        return TheFalseObj;
    }
    return TheTrueObj;
}


/** Menu::getAutoConnectStatus()
 *
 *  Returns a Directory object containing the current stats of autoconnection.
 *
 *  @return  A Directory object with 2 indexes:
 *
 *           autoConnect -> .true if autoconnection is on and .false if it is
 *           off.
 *           methodName -> .nil if not set, othewise the name of the
 *                         autoconnection method.
 */
RexxMethod1(RexxObjectPtr, menu_getAutoConnectStatus, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);
    return (RexxObjectPtr)cMenu->autoConnectionStatus();
}


/** Menu::itemTextToMethodName()
 *
 * Converts the text of a (presumed) menu command it to a valid Rexx method
 * name.  That is, it removes all space characters, the possible ampersand, any
 * trailing '...' and upper-cases it.
 *
 * This would normally be done when connecting a menu command item to a method.
 *
 * @param  text  The text to convert.
 *
 * @return the string that is valid to use as a method name.
 */
RexxMethod2(RexxStringObject, menu_itemTextToMethodName, CSTRING, text, OSELF, self)
{
    RexxStringObject result = NULLOBJECT;

    if ( strlen(text) == 0 )
    {
        nullStringMethodException(context, 1);
        return result;
    }

    char *name = strdup_2methodName(text);
    if ( name != NULL )
    {
        result = context->String(name);
        free(name);
    }
    else
    {
        outOfMemoryException(context->threadContext);
    }
    return result;
}


/** Menu::connectMenuEvent()
 *
 * Connects WM_XXX menu message events to a method in a dialog.
 *
 * @param methodName  The method name of the method being connected.
 *
 * @param keyword     A single keyword specifying which message event to
 *                    connect.
 *
 * @param hFilter     Window handle of an object to use for to filter the
 *                    connected notification. The use of this filter is specific
 *                    to the event being connectd.  For the CONTEXTMENU event it
 *                    can be a window handle, in which case only notifications
 *                    for that window are generated.  For the INITMENUPOPUP
 *                    event it can be the menu handle of a submenu, in which
 *                    case only notifications for that submenu are generated.
 *
 *
 * @param dlg         [optional] The dialog being connected.  By default the
 *                    dialog this menu is attached to is used.  However, any
 *                    dialog can be used.  In most cases, it only makes sense to
 *                    connect the window message to the dialog the menu is
 *                    attached to.
 *
 *                    If omitted and there is no owner dialog, then a condition
 *                    is raised.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and the
 *        menu does not have an assigned dialog.
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dialog argument was not
 *        ommitted, but the object is not a .PlainBaseDialog, (or subclass of
 *        course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 */
RexxMethod5(logical_t, menu_connectMenuEvent, CSTRING, methodName, CSTRING, keyWord,
            OPTIONAL_POINTERSTRING, hFilter, OPTIONAL_RexxObjectPtr, _dlg, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->connectMenuMessage(methodName, keyWord, (HWND)hFilter, _dlg);
}

/** Menu::connectCommandEvent()
 *
 * Directly connects the menu command item event with a method in the specified
 * dialog.
 *
 * @param id          The resource ID of the menu item.
 *
 * @param methodName  The method to connect the item select event to.  This can
 *                    not be the empty string.
 *
 * @param dlg         [optional] The dlg to connect the menu item select event
 *                    to. If omitted, then the menu's owning dialog is used. If
 *                    omitted and there is no owning dialog, no connection is
 *                    made and the system error code is set as mentioned below.
 *                    (To connect a menu item select event, there must be a
 *                    dialog to connect it to.)
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and the
 *        menu does not have an assigned dialog.
 *
 *        ERROR_INVALID_PARAMETER (87) -> The methodName argument can not be the
 *        empty string.
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dialog argument was not
 *        ommitted, but the object is not a .PlainBaseDialog, (or subclass of
 *        course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 */
RexxMethod4(logical_t, menu_connectCommandEvent, RexxObjectPtr, rxID, CSTRING, methodName,
            OPTIONAL_RexxObjectPtr, _dlg, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->connectCommandEvent(rxID, methodName, _dlg);
}


/** Menu::connectAllCommandEvents()
 *
 * Connects all menu command items in this menu to a method.
 *
 * @param  msg   [optional]  Connect all menu command items to the method by
 *               this name.  The default is to connect all menu command items to
 *               a method name composed from the text of the command item.
 *
 * @param  _dlg  [optional]  Connect the command items to the method(s) of this
 *               dialog object.  The default is to connect the command items to
 *               the owner dialog of this menu.
 *
 * @return       True on success, false on error.
 *
 * @note  Sets .SystemErrorCode.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and the
 *        menu does not have an assigned dialog.
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dialog argument was not
 *        ommitted, but the object is not a .PlainBaseDialog, (or subclass of
 *        course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 */
RexxMethod3(logical_t, menu_connectAllCommandEvents, OPTIONAL_CSTRING, msg, OPTIONAL_RexxObjectPtr, _dlg, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->connectAllCommandEvents(msg, _dlg);
}

/** Menu::connectSomeCommandEvents()
 *
 * Connects a collection of menu command items to the single specified method.

 * @param rxItemIDs   A collection of item IDs to connect.  The IDs can be by
 *                    position IDs or resource IDs, depending on the value of
 *                    byPosition. However, they must be all the same type, all
 *                    resource IDs or all by position IDs.
 *
 * @param  msg        [optional]  Connect all menu command items to the method
 *                    by this name.  The default is to connect all menu command
 *                    items to a method name composed from the text of the
 *                    command item.
 *
 *                    If not omitted, method can not be the empty string.
 *
 * @param byPosition  [optional]  If true, rxItemIDs are positional IDs,
 *                    otherwise the are resource IDs. The default is false,
 *                    resource IDs.
 *
 * @param  _dlg       [optional]  Connect the command items to the method(s) of
 *                    this dialog object.  The default is to connect the command
 *                    items to the owner dialog of this menu.
 *
 * @return       True on success, false on error.
 *
 * @note  Sets .SystemErrorCode.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and the
 *        menu does not have an assigned dialog.
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dialog argument was not
 *        ommitted, but the object is not a .PlainBaseDialog, (or subclass of
 *        course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 *
 *        ERROR_INVALID_PARAMETER (87) -> One or more of the specified item IDs
 *        is not a menu command item.  Or the msg argument was used, but it is
 *        the empty string.
 *
 *        The method quits when it encounters the first error, therefore menu
 *        items processed before the error will be connected and menu items that
 *        would be processed after the error will not be connected.
 */
RexxMethod5(logical_t, menu_connectSomeCommandEvents, RexxObjectPtr, rxItemIDs, OPTIONAL_CSTRING, msg,
            OPTIONAL_logical_t, byPosition, OPTIONAL_RexxObjectPtr, _dlg, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->connectSomeCommandEvents(rxItemIDs, msg, byPosition, _dlg);
}


/**
 *  Methods for the .MenuBar mixin class.
 */
#define MENUBAR_CLASS       "MenuBar"


/** MenuBar::attachTo()
 *
 * Attaches this menu bar to the specified dialog window.
 *
 * A menu bar can only be attached to one dialog and a dialog can only have one
 * menu bar attached to it.  If either the menu bar is already attached to a
 * dialog, or the dialog already has a menu bar attached to it, this method
 * fails.
 *
 * If the menu bar is already attached to a dialog, first use the
 * .MenuBar~detach() method to detach it.
 *
 * If the dialog already has a menu bar attached to it and you want to attach
 * this menu bar to that dialog, get a reference to the dialog's current menu
 * bar and use the replace() method.
 *
 *  @param  dlg         The dialog to attach to.
 *
 *  @param  countRows   [optional] The number of rows the menu has. If this
 *                      number is greater than 0, the size of the dialog is
 *                      automatically adjusted to account for the height of the
 *                      menu.
 *
 *  @return  True on success, false on failure.
 *
 *  @note  Sets .SystemErrorCode on failure.  In addition to error codes set by
 *         the operating system, the following error codes may be set by
 *         ooDialog:
 *
 *            ERROR_INVALID_FUNCTION (1) this menu is already attached to a dlg
 *
 *            ERROR_NOT_ENOUGH_MEMORY (8) some menu items were not connected
 *            because the message table is full.
 *
 *            ERROR_INVALID_WINDOW_HANDLE (1400) dlg has no underlying Windows
 *            dialog
 *
 *            ERROR_INVALID_MENU_HANDLE (1401) this menu has been destroyed
 *
 *            ERROR_WINDOW_NOT_DIALOG (1420) dlg is not a .PlainBaseDialog
 *
 *            ERROR_INVALID_WINDOW_STYLE (2002) dlg already attached to a menu
 *            bar
 *
 *         When this method returns false, the menu is not attached to a dialog,
 *         except in one circumstance.  If the .SystemErrorCode is
 *         ERROR_NOT_ENOUGH_MEMORY, then the menu is attached to the dialog, but
 *         some menu items were not connected.
 */
RexxMethod3(logical_t, menuBar_attachTo, RexxObjectPtr, dlg, OPTIONAL_uint32_t, countRows, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->attachToDlg(dlg, countRows);
}

/** MenuBar::detach()
 *
 *  Detaches this menu from its assigned dialog.
 *
 *  @return  True on success, false on failure.
 *
 *  @note  Sets .SystemErrorCode on failure.  In addition to codes set by the
 *         operating system, the following error code indicates that this menu
 *         bar is not attached to a dialog:
 *
 *         ERROR_INVALID_FUNCTION (1) -> Not attached to a dialog.
 */
RexxMethod1(logical_t, menuBar_detach, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->detach(false);
}

/** MenuBar::isAttached()
 *
 * Determines if this menu bar is currently attached to a dialog.
 *
 * @return  True if attached, false if not attached.
 */
RexxMethod1(logical_t, menuBar_isAttached, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);
    oodResetSysErrCode(context->threadContext);
    return cMenu->isAttached();
}


/** MenuBar::redraw()
 *
 *  Tells the dialog this menu bar is attached to, to redraw the menu.
 *
 *  @return  True on success, false on failure.
 *
 *  @note  Sets .SystemErrorCode on failure.  In addition to codes set by the
 *         operating system, the following error code indicates that this menu
 *         bar is not attached to a dialog:
 *
 *         ERROR_INVALID_FUNCTION (1) -> Not attached to a dialog.
 */
RexxMethod1(logical_t, menuBar_redraw, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    oodResetSysErrCode(context->threadContext);
    return cMenu->maybeRedraw(true);
}


/** MenuBar::replace()
 *
 *  If this menubar is attached to a dialog, the menubar for the dialog is
 *  replaced by the specified menubar.
 *
 *  @param   newMenu  The new menu bar to attach to this menu bar's dialog.
 *
 *  @return  The existing menu bar, if there is one, otherwise .ni
 *
 *  @note  Sets .SystemErrorCode on failure.  In addition to codes set by the
 *         operating system, the following error code indicates that this menu
 *         bar is not attached to a dialog:
 *
 *         ERROR_INVALID_FUNCTION (1) -> Not attached to a dialog.
 */
RexxMethod2(RexxObjectPtr, menuBar_replace, RexxObjectPtr, newMenu, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->replace(newMenu);
}


/**
 *  Methods for the .Menu mixin class.
 */
#define MENUTEMPLATE_CLASS       "MenuTemplate"


/** MenuTemplate::addPopup()
 *
 * Adds a popup menu to the in memory template.  This method is only valid after
 * the .UserMenu has been created and before it is completed.
 *
 * @param id      The resource ID for the popup menu.
 *
 * @param text    The text label for the popup menu.
 *
 * @param opts    [optional]  A string of 0 or more, blank seperated, keywords
 *                indicating additional options for the popup menu.  The
 *                keywords set the state and type of the of the popup menu, and
 *                specify when the popup menu is the last item at the current
 *                level in the menu.
 *
 * @param helpID  [optional]  The context help ID for the popup menu.  The
 *                default is 0 indicating no help ID.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The .UserMenu has already been
 *        completed.
 *
 * @note  State keywords: DEFAULT DISABLED GRAYED HILITE.  The keywords:
 *        ENABLED UNHILITE NOTDEFAULT, although valid keywords are simply
 *        ignored. By default the state will be ENABLED UNHILITE NOTDEFAULT
 *
 *        Type keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY RIGHTORDER
 *
 *        Additional keyword: END  This keyword is required to indicate the
 *        popup menu is the last item at the current level of the menu.
 *
 * @remarks  Note to myself. I have tested this with DEFAULT, DISABLED, GRAYED,
 *           HILITE, MENUBARBREAK, MENUBREAK, RIGHTJUSTIFY, and RIGHTORDER. They
 *           all work.
 */
RexxMethod5(logical_t, menuTemplate_addPopup, RexxObjectPtr, rxID, CSTRING, text,
            OPTIONAL_CSTRING, opts, OPTIONAL_RexxObjectPtr, rxHelpID, OSELF, self)
{
    CppMenu *cMenu = menuToCSelf(context, self);
    cMenu->setContext(context, TheFalseObj);

    return cMenu->addTemplatePopup(rxID, text, opts, rxHelpID);
}

/** MenuTemplate::addItem()
 *
 * Adds a menu command item to the in memory template.  This method is only
 * valid after the .UserMenu has been created and before it is completed.
 *
 * @param id      The resource ID for the menu command item.
 *
 * @param text    The text label for the item.
 *
 * @param opts    [optional]  A string of 0 or more, blank seperated, keywords
 *                indicating additional options for the menu item.  The keywords
 *                set the state and type of the of the item, and specify when
 *                the item is the last item at the current level in the menu.
 *
 * @param method  [optional]  A method name to connect the item to.  The default
 *                is to not connect the menu command item. If this argument is
 *                used it can not be the empty string.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The .UserMenu has already been
 *        completed.
 *
 *        ERROR_INVALID_PARAMETER (87) -> Argument method can not be the empty
 *        string.
 *
 * @note  State keywords: DEFAULT DISABLED GRAYED HILITE CHECKED.  The keywords:
 *        ENABLED UNHILITE NOTDEFAULT UNCHECKED, although valid keywords are
 *        simply ignored. By default the state will be ENABLED UNHILITE
 *        NOTDEFAULT UNCHECKED.
 *
 *        Type keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY RADIO.  By default
 *        no special type is set.
 *
 *        Additional keyword: END  This keyword is required to indicate the menu
 *        command item is the last item at the current level of the menu.
 *
 */
RexxMethod5(logical_t, menuTemplate_addItem, RexxObjectPtr, rxID, CSTRING, text,
            OPTIONAL_CSTRING, opts, OPTIONAL_CSTRING, method, OSELF, self)
{
    CppMenu *cMenu = menuToCSelf(context, self);
    cMenu->setContext(context, TheFalseObj);

    return cMenu->addTemplateItem(rxID, text, opts, method);
}


/** MenuTemplate::addSeparator()
 *
 * Adds a separator item to the in memory template.  This method is only valid
 * after the .UserMenu has been created and before it is completed.
 *
 * @param id      The resource ID for the separator.
 *
 * @param opts    [optional]  A string of 0 or more, blank seperated, keywords
 *                indicating additional options for the menu item.  The keywords
 *                specify a few of the additional type settings that work with a
 *                separator and can indicate that the separator is the last item
 *                at the current level in the menu.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The .UserMenu has already been
 *        completed.
 *
 * @note  Type keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY.  By default no
 *        special type is set.
 *
 *        Additional keyword: END  This keyword is required to indicate the menu
 *        command item is the last item at the current level of the menu.
 *
 * @remarks  Although MSDN says separators can not be used in a menu bar, on
 *           Windows 7 at least they can. Not sure if that is because of using
 *           the extended menu template, or a Windows 7 thing only
 *
 */
RexxMethod3(logical_t, menuTemplate_addSeparator, RexxObjectPtr, rxID, OPTIONAL_CSTRING, opts, OSELF, self)
{
    CppMenu *cMenu = menuToCSelf(context, self);
    cMenu->setContext(context, TheFalseObj);

    return cMenu->addTemplateSepartor(rxID, opts);
}


/** MenuTemplate::isComplete()
 *
 *  Determines if the in memory template has been loaded into memory as a menu
 *  resource.  Once this is done, no more menu items can be added to the menu,
 *  through the addItem(), addSeparator(), and addPopup() methods.
 *
 *  Those items can still be added to the menu through the insertItem(),
 *  insertSeparator(), and insertPopup() methods.
 *
 *  @return  True if the memory template has been finished and loaded into
 *           memory, otherwise false.
 */
RexxMethod1(logical_t, menuTemplate_isComplete, OSELF, self)
{
    CppMenu *cMenu = menuToCSelf(context, self);
    cMenu->setContext(context, TheFalseObj);
    return cMenu->templateIsComplete();
}


/**
 *  Methods for the .BinaryMenuBar class.
 */
#define BINARYMENUBAR_CLASS       "Menu"

/** BinaryMenuBar::init()
 *
 *  Initializes a BinaryMenuBar object.
 *
 *  @param  src          [optional]  The source to create the menu.  Can be:
 *                       .nil       -> Create new empty menu
 *                       .Pointer   -> Must be a valid HMENU
 *                       .ResDialog -> Load menu from the resource DLL
 *                       .String    -> File name of module to load menu from
 *                       The default if omitted is to create a new empty menu.
 *
 *  @param  id           [optional]  The resource ID of the menu.  The default
 *                       is -1 indicating no ID.  However, the ID is required if
 *                       src is a .ResDialog or a module file name.
 *
 *  @param  helpID       [optional]  Context help ID for the menu.  The default
 *                       is 0 indicating no help ID.
 *
 *  @param  attachTo     [optional]  If specified, attach this menu to the
 *                       dialog. If specified, attachTo has to be a
 *                       .PlainBaseDialog or subclass.  If not, an exception is
 *                       raised.
 *
 *  @param  autoConnect  [optional]  Turn on auto connection if true.  Default
 *                       is false
 *
 *  @param  mName        [optional]  If autoConnect, set autoConnectionMethod to
 *                       this method.  If omitted autoConectionMethod is left
 *                       alone.
 *
 *  @return  No return.
 *
 *  @remarks  Raises exceptions for all failures.  If no exceptions, the menu is
 *            created successfully.
 *
 *            Also sets .SystemErrorCode.  It will be non-zero, only, if the
 *            help ID is set and there is a Windows API failure.  It hardly
 *            seems possible that this could happen.
 */
RexxMethod7(RexxObjectPtr, binMenu_init, OPTIONAL_RexxObjectPtr, src, OPTIONAL_RexxObjectPtr, _id,
            OPTIONAL_RexxObjectPtr, helpID, OPTIONAL_RexxObjectPtr, attachTo, OPTIONAL_logical_t, autoConnect,
            OPTIONAL_CSTRING, mName, OSELF, self)
{
    RexxMethodContext *c = context;

    CppMenu *cMenu = new CppMenu(self, BinaryMenuBar, context);
    RexxPointerObject cMenuPtr = context->NewPointer(cMenu);
    context->SendMessage1(self, "MENUINIT", cMenuPtr);

    if ( argumentExists(2) )
    {
        int32_t id = oodGlobalID(context, _id, 2, false);
        if ( id == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        cMenu->setMenuID(id);
    }

    DWORD dwHelpID = 0;
    if ( argumentExists(3) )
    {
        int32_t tmp = oodGlobalID(context, helpID, 3, false);
        if ( tmp == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        dwHelpID = tmp;
    }

    // We check here, before we go any farther, if the attachTo argument is
    // valid and attachToDlg() is not going fail later:
    if ( argumentExists(4) )
    {
        if ( ! validAttachTo(context, attachTo, 4) )
        {
            goto done_out;
        }
    }

    HMENU hMenu = NULL;
    bool isResDialog = false;
    bool isPointer = false;

    if ( argumentOmitted(1) || src == TheNilObj )
    {
        hMenu = CreateMenu();
        if ( hMenu == NULL )
        {
            systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "CreateMenu");
            goto done_out;
        }
    }
    else if ( context->IsPointer(src) )
    {
        isPointer = true; // Do not destroy this menu if we fail.

        hMenu = (HMENU)context->PointerValue((RexxPointerObject)src);
        if ( ! IsMenu(hMenu) )
        {
            invalidTypeException(context->threadContext, 1, INVALID_MENU_HANDLE_MSG);
            goto done_out;
        }
    }
    else if ( context->IsOfType(src, "ResDialog") )
    {
        isResDialog = true;
    }
    else if ( context->IsString(src) )
    {
        ;  // Purposively do nothing because src could be a module file name.
    }
    else
    {
        wrongArgValueException(context->threadContext, 1, "ResDialog, module file name, menu handle, or .nil", src);
        goto done_out;
    }

    if ( hMenu == NULL )
    {
        // src has to be a ResDialog or a module file name.
        HINSTANCE hinst = NULL;

        if ( isResDialog )
        {
            pCPlainBaseDialog pcpbd = dlgToCSelf(context, src);
            hinst = pcpbd->hInstance;
        }
        else
        {
            CSTRING fileName = c->ObjectToStringValue(src);
            hinst = LoadLibrary(TEXT(fileName));
            if ( hinst == NULL )
            {
                systemServiceExceptionCode(context->threadContext, NO_HMODULE_MSG, fileName);
                goto done_out;
            }
        }

        hMenu = LoadMenu(hinst, MAKEINTRESOURCE(cMenu->wID));
        if ( hMenu == NULL )
        {
            systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "LoadMenu");
            goto done_out;
        }

        if ( ! isResDialog )
        {
            FreeLibrary(hinst);
        }
    }

    cMenu->setHMenu(hMenu);

    if ( dwHelpID != 0 )
    {
        uint32_t rc = menuHelpID(hMenu, dwHelpID, TRUE, NULL);
        if ( rc )
        {
            oodSetSysErrCode(context->threadContext, rc);
        }
    }

    if ( autoConnect )
    {
        if ( ! cMenu->setAutoConnection(TRUE, mName) )
        {
            outOfMemoryException(c->threadContext);
            goto done_out;
        }
    }

    if ( argumentExists(4) )
    {
        cMenu->attachToDlg(attachTo, 0);
    }

    // TODO need to think about putting this Rexx object in the data word of the menu.

done_out:
    return NULLOBJECT;
}


/**
 *  Methods for the .SystemMenu class.
 */
#define SYSTEMMENU_CLASS       "SystemMenu"


/** SystemMenu::init()
 *
 *  Initializes a .SystemMenu object.  The underlying menu object is a copy of
 *  the system menu of the specified dialog.  Once the copy is made, the system
 *  menu can be manipulated and modified just like other menu objects.
 *
 *  It should be obvious that the dialog must have a system menu in order to
 *  instantiate a .SystemMenu object.
 *
 *  @param  dialog     The dialog whose system menu is to be copied.
 *
 *  @note  Raises exceptions for all failures.  If no exceptions, the menu is
 *         created successfully.
 */
RexxMethod2(RexxObjectPtr, sysMenu_init, RexxObjectPtr, dialog, OSELF, self)
{
    CppMenu *cMenu = new CppMenu(self, SystemMenu, context);
    RexxPointerObject cMenuPtr = context->NewPointer(cMenu);
    context->SendMessage1(self, "MENUINIT", cMenuPtr);

    if ( ! requiredClass(context->threadContext, dialog, "PLAINBASEDIALOG", 1) )
    {
        goto done_out;
    }

    cMenu->setUpSysMenu(dialog);

done_out:
    return NULLOBJECT;
}


/** SystemMenu::revert()
 *
 * Reverts the system menu for the dialog back to the standard system menu.
 *
 * When a SystemMenu object is instantiated, it receives a copy of the system
 * menu for the dialog.  (The operating system maintains the standard system
 * menu.) This method reverses that process.  The operating system removes the
 * copied system menu and replaces it with the standard system menu.
 *
 * This object will then no loger be valid.  Note however that any of the
 * pre-defined System Command menu items that were connected to methods, will
 * remain connected.  Currently in ooDialog there is no way to 'unconnect' a
 * method connection once it is made.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The operating system will only revert back to the standard system
 *        menu. A dialog that does not have minimize and maximize buttons is
 *        originally given a standard system menu, where the operating system
 *        has removed the menu items that are not applicable.  Namely, the
 *        Restore, Size, Maximize and Minimize menu items.
 *
 *        When the operating system reverts back to the standard system menu on
 *        these dialogs, it does not remove these menu items.  There is nothing
 *        the programmer can do about this, because only the operating system
 *        has access to the standard system menu.
 */
RexxMethod1(logical_t, sysMenu_revert, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->revertSysMenu();
}


/**
 *  Methods for the .PopupMenu class.
 */
#define POPUPMENU_CLASS       "PopupMenu"


/** PopupMenu::connectContextMenu() [class]
 *
 * This class method connects WM_CONTEXTMENU Windows message event to a method
 * in a dialog. The WM_CONTEXTMENU event is generated when the user right-mouse
 * clicks on a window.
 *
 * @param dlg         The dialog being connected.
 *
 * @param methodName  The method name of the method being connected.
 *
 * @param hwnd        [optional] A window handle to filter the right-clicks on.
 *                    This can be the window handle of any control in the dialog
 *                    being connnected, or even the dialog window handle itself.
 *                    If used, only right-clicks on the specified window will be
 *                    received.  If omitted, all right-clicks on the dialog are
 *                    received.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dlg argument not a
 *        .PlainBaseDialog, (or subclass of course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 */
RexxMethod3(logical_t, popMenu_connectContextMenu_cls, RexxObjectPtr, dlg, CSTRING, methodName, OPTIONAL_POINTERSTRING, hwnd)
{
    BOOL success = FALSE;

    pCEventNotification pcen = _getPCEN(context, dlg);
    if ( pcen == NULL )
    {
        goto done_out;
    }

    uint32_t tag = TAG_MENU;

    if ( hwnd != NULL )
    {
        success = addMiscMessage(pcen, context, WM_CONTEXTMENU, UINT32_MAX, (WPARAM)hwnd, UINTPTR_MAX, 0, 0, methodName, tag);
    }
    else
    {
        success = addMiscMessage(pcen, context, WM_CONTEXTMENU, UINT32_MAX, 0, 0, 0, 0, methodName, tag);
    }

    if ( ! success )
    {
        oodSetSysErrCode(context->threadContext, ERROR_NOT_ENOUGH_MEMORY);
    }

done_out:
    return success;
}


/** PopupMenu::init()
 *
 *  Initializes a .PopupMenu object.  The underlying menu object is either
 *  created as a new empty popup menu, or an existing popup menu handle is
 *  passed in.
 *
 *  @param  id           [optional]  The resource id of / for the menu.  Can be
 *                       -1 to indicate no ID.  The default is -1.
 *
 *  @param  helpID       [optional]  Context help ID for the menu.  The default
 *                       is 0 indicating no help ID.
 *
 *  @param  handle       [optional]  If not null or omitted, this is the handle
 *                       of an existing submenu.  This is then set as the
 *                       underlying menu of this .PopupMenu.  When omitted or
 *                       null, a new empty menu is created as the underlying
 *                       menu.
 *
 *                       TODO - don't document this argument at this time.  The
 *                       user has no way of getting a menu handle, except
 *                       through a menu object.  If the user gets a submenu from
 *                       a menu bar, or other menu, to get its handle, it is
 *                       easier just to use the submenu object itself.  So,
 *                       there seems to be no use for this argument, at this
 *                       time.
 *
 *  @return  No return.
 *
 *  @remarks  Raises exceptions for all failures.  If no exceptions, the menu is
 *            created successfully.
 *
 *            Also sets .SystemErrorCode.  It will be non-zero, only, if the
 *            help ID is set and there is a Windows API failure.  It hardly
 *            seems possible that this could happen.
 */
RexxMethod4(RexxObjectPtr, popMenu_init, OPTIONAL_RexxObjectPtr, _id, OPTIONAL_RexxObjectPtr, helpID,
            OPTIONAL_POINTER, handle, OSELF, self)
{
    CppMenu *cMenu = new CppMenu(self, PopupMenu, context);
    RexxPointerObject cMenuPtr = context->NewPointer(cMenu);
    context->SendMessage1(self, "MENUINIT", cMenuPtr);

    if ( argumentExists(1) )
    {
        int32_t id = oodGlobalID(context, _id, 1, false);
        if ( id == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        else
        {
            cMenu->setMenuID(id);
        }
    }

    DWORD dwHelpID = 0;
    if ( argumentExists(2) )
    {
        int32_t tmp = oodGlobalID(context, helpID, 2, false);
        if ( tmp == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        dwHelpID = tmp;
    }

    HMENU hMenu = NULL;
    if ( handle == NULL )
    {
        hMenu = CreatePopupMenu();
        if ( hMenu == NULL )
        {
            systemServiceExceptionCode(context->threadContext, API_FAILED_MSG, "CreatePopupMenu");
            goto done_out;
        }
    }
    else
    {
        hMenu = (HMENU)handle;
        if ( ! IsMenu(hMenu) )
        {
            invalidTypeException(context->threadContext, 4, INVALID_MENU_HANDLE_MSG);
            goto done_out;
        }
    }

    cMenu->setHMenu(hMenu);

    if ( dwHelpID != 0 )
    {
        uint32_t rc = menuHelpID(hMenu, dwHelpID, TRUE, NULL);
        if ( rc )
        {
            oodSetSysErrCode(context->threadContext, rc);
        }
    }

    // TODO need to think about putting this Rexx object in the data word of the menu.

done_out:
    return NULLOBJECT;
}


/** PopupMenu::connectContextMenu()
 *
 * Connects WM_CONTEXTMETHOD messages to a method in a dialog.
 *
 * @param methodName  The method name of the method being connected.
 *
 * @param hwnd        [optional] A window handle to filter the right-clicks on.
 *                    This can be the window handle of any control in the dialog
 *                    being connnected, or even the dialog window handle itself.
 *                    If used, only right-clicks on the specified window will be
 *                    received.  If omitted, all right-clicks on the dialog are
 *                    received.
 *
 * @param dlg         [optional] The dialog being connected.  By default the
 *                    dialog this short cut menu is assigned to is used.
 *                    However, any dialog can be used.  This especially makes
 *                    sense if the track() method is being used for the short
 *                    cut menu.
 *
 *                    If omitted and there is no assigned dialog, then a
 *                    condition is raised.
 *
 * @return  True on success, false on error.
 *
 * @note  Sets .SystemErrorCode on error.
 *
 *        The system error code is set this way in addition to what the OS might
 *        set:
 *
 *        ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and the
 *        menu does not have an assigned dialog.
 *
 *        ERROR_WINDOW_NOT_DIALOG (1420) -> The dialog argument was not
 *        ommitted, but the object is not a .PlainBaseDialog, (or subclass of
 *        course.)
 *
 *        ERROR_NOT_ENOUGH_MEMORY (8) -> The dialog message table is full.
 */
RexxMethod4(logical_t, popMenu_connectContextMenu, CSTRING, methodName, OPTIONAL_POINTERSTRING, hwnd,
            OPTIONAL_RexxObjectPtr, _dlg, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->connectMenuMessage(methodName, "CONTEXTMENU", (HWND)hwnd, _dlg);
}

/** PopupMenu::isAssigned()
 *
 * Determines if this short cut menu is currently assigned to a dialog.
 *
 * @return  True if attached, false if not attached.
 */
RexxMethod1(logical_t, popMenu_isAssigned, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);
    oodResetSysErrCode(context->threadContext);
    return cMenu->isAssigned();
}


/** PopupMenu::assignTo()
 *
 * For short cut, also called context, menus only.
 *
 * Assigns this shortcut menu to a dialog. When a popup menu is used as a
 * shortcut (context) menu it sends messages to its owner dialog when menu items
 * are selected.  Even if the short cut menu is only used with the track()
 * method, (where selecting a menu item does not send a message to its owner
 * dialog,) the short cut menu still requires an owner dialog.
 *
 * It is not neccessary to assign a short cut menu to a dialog, it just may be
 * more convenient.  If the shortcut menu does not have an assigned dialog, then
 * the owner dialog must be specified each time the track() or show() methods
 * are invoked.
 *
 * A short cut menu can only be assigned to one dialog, so if this menu is
 * already assigned to a dialog, it is simply replaced.
 *
 *  @param  dlg         The dialog to assign this short cut menu to.
 *  @param  connect     [optional] Turns on autoconnection.
 *  @param  methodName  [optional] Method name for autoconnection.
 *
 *  @return  True on success, false on failure.
 *
 *  @note  Sets .SystemErrorCode on failure.  In addition to error codes set
 *         bythe operating system, the following error codes may be set:
 *
 *            ERROR_INVALID_MENU_HANDLE (1401) this menu has been destroyed
 *
 *            ERROR_WINDOW_NOT_DIALOG (1420) dlg is not a .PlainBaseDialog
 */
RexxMethod4(logical_t, popMenu_assignTo, RexxObjectPtr, dlg, OPTIONAL_logical_t, autoConnect,
            OPTIONAL_CSTRING, methodName, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->assignToDlg(dlg, autoConnect, methodName);
}


/** PopupMenu::track()
 *
 *  Displays a contex menu and returns after the user has selected an item or
 *  canceled the menu.  No notification (no WM_COMMAND message) is sent to the
 *  owner window, but rather the selected id is returned directly.  0 is
 *  returned if the user cancels the menu.
 *
 *  @param  location     A Point object specifying the location of the shortcut
 *                       menu, in screen co-ordinates.
 *
 *  @param  ownerDlg     [optional]
 *
 *  @param  opts         [optional]  A string containing blank separated
 *                       keywords specifying additional options for the shortcut
 *                       menu.
 *
 *  @param  bothButtons  [optional]  If true, the user can select menu items
 *                       with both the left and right mouse buttons. If false,
 *                       the default, the user can select menu items with only
 *                       the left mouse button.
 *
 *  @param  excludeRect  [optional]  A Rect object that specifies an area of the
 *                       screen the shortcut menu should not overlap.
 *
 *  @return  A non-negative number on success, -1 on error.  On success, the
 *           return is either 0, meaning the user canceled the menu, or the id
 *           of the menu item selected.
 *
 *  @notes Sets .SystemErrorCode. The system error code is set this way:
 *
 *         ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and the
 *         menu does not have an assigned dialog.
 *
 *         ERROR_WINDOW_NOT_DIALOG (1420) The dialog argument was specified, but
 *         the object is not a .PlainBaseDialog, (or subclass of course.)
 *
 *         ERROR_INVALID_MENU_HANDLE (1401) This menu has been destroyed, or is
 *         no longer valid for some reason.
 *
 *         ERROR_INVALID_WINDOW_HANDLE (1400) The window handle for the dialog
 *         could not be obtained.
 *
 */
RexxMethod6(RexxObjectPtr, popMenu_track, RexxObjectPtr, location, OPTIONAL_RexxObjectPtr, _dlg,
            OPTIONAL_CSTRING, opts, OPTIONAL_logical_t, bothButtons, OPTIONAL_RexxObjectPtr, excludeRect, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->trackPopup(location, _dlg, opts, bothButtons, excludeRect, true);
}


/** PopupMenu::show()
 *
 *  Displays a contex menu and returns after the user has selected an item or
 *  canceled the menu.  When the user selects an item, a WM_COMMAND message with
 *  the id of the selection is generated.  If the user cancels, nothing happens.
 *
 *  @param  location     A Point object specifying the location of the shortcut
 *                       menu, in screen co-ordinates.
 *
 *  @param  ownerDlg     [optional]
 *
 *  @param  opts         [optional]  A string containing blank separated
 *                       keywords specifying additional options for the shortcut
 *                       menu.
 *
 *  @param  bothButtons  [optional]  If true, the user can select menu items
 *                       with both the left and right mouse buttons. If false,
 *                       the default, the user can select menu items with only
 *                       the left mouse button.
 *
 *  @param  excludeRect  [optional]  A Rect object that specifies an area of the
 *                       screen the shortcut menu should not overlap.
 *
 *  @return  True on success, othewise false.
 *
 *  @notes   Sets .SystemErrorCode.  In addition to error codes set by the
 *           operating system, the system error code may be set this way:
 *
 *           ERROR_INVALID_FUNCTION (1) -> The dialog argument was omitted and
 *           the menu does not have an assigned dialog.
 *
 *           ERROR_WINDOW_NOT_DIALOG (1420) The dialog argument was specified,
 *           but the object is not a .PlainBaseDialog, (or subclass of course.)
 *
 *           ERROR_INVALID_MENU_HANDLE (1401) This menu has been destroyed, or
 *           is no longer valid for some reason.
 *
 *           ERROR_INVALID_WINDOW_HANDLE (1400) The window handle for the dialog
 *           could not be obtained.
 */
RexxMethod6(RexxObjectPtr, popMenu_show, RexxObjectPtr, location, OPTIONAL_RexxObjectPtr, _dlg,
            OPTIONAL_CSTRING, opts, OPTIONAL_logical_t, bothButtons, OPTIONAL_RexxObjectPtr, excludeRect, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    return cMenu->trackPopup(location, _dlg, opts, bothButtons, excludeRect, false);
}


/**
 *  Methods for the .ScriptMenuBar class.
 */
#define SCRIPTMENUBAR_CLASS       "ScriptMenuBar"


/** ScriptMenuBar::init()
 *
 *  Initializes a .ScriptMenuBar object.  The underlying menu object is created
 *  in memory by parsing a resource script file.  The script is parsed and
 *  converted to a HMENU at once.
 *
 *  @param  rcFile       [required]  The file name of the resource script.
 *
 *  @param  id           [optional]  The resource id of the menu.  If omitted,
 *                       then the first menu found in the resource file is used.
 *
 *  @param  helpID       [optional]  Context help ID for the menu.  The default
 *                       is 0 indicating no help ID.
 *
 *  @param  count        [optional]  The count of menu items.  This determines
 *                       the size of memory allocated for the template. The
 *                       default if omitted is 100.
 *
 *  @param  connect      [optional]  If true, each menu command item in the menu
 *                       is connected to a method in a Rexx dialog.  The name of
 *                       the method is composed from the menu item text.  This
 *                       uses the connection requested method of connecting menu
 *                       items. The default is false.
 *
 *  @param  attachTo     [optional]  If specified attach this menu to the
 *                       dialog.  If specified, attachTo has to be a
 *                       .PlainBaseDialog or subclass.  If not, an exception is
 *                       raised.
 *
 *  @return  No return.
 *
 *  @remarks  Raises exceptions for all failures.  If no exceptions, the menu is
 *            created successfully.
 *
 *            Also sets .SystemErrorCode.  It will be non-zero, only, if the
 *            help ID is set and there is a Windows API failure.  It hardly
 *            seems possible that this could happen.
 *
 *            Script menu bars pose some problems relating to the MENU
 *            definition ID in the resource script file.  The menu id can be a
 *            simple string.  Because of that, we can't raise an exception if
 *            the ID does not resolve, because it could be a legimate string
 *            name for the menu.  We need to wait and have load() raise an
 *            exception if it can not locate the menu.
 */
RexxMethod7(RexxObjectPtr, scriptMenu_init, RexxStringObject, rcFile, OPTIONAL_RexxObjectPtr, _id,
            OPTIONAL_RexxObjectPtr, helpID, OPTIONAL_uint32_t, count, OPTIONAL_logical_t, connect,
            OPTIONAL_RexxObjectPtr, attachTo, OSELF, self)
{
    CppMenu *cMenu = new CppMenu(self, ScriptMenuBar, context);
    RexxPointerObject cMenuPtr = context->NewPointer(cMenu);
    context->SendMessage1(self, "MENUINIT", cMenuPtr);

    bool idOmitted = argumentOmitted(2) ? true : false;

    if ( argumentExists(2) )
    {
        int32_t id = oodGlobalID(context, _id, 2, true);
        if ( id == OOD_ID_EXCEPTION )
        {
            context->ClearCondition();
        }
        else
        {
            cMenu->setMenuID(id);
        }
    }

    DWORD dwHelpID = 0;
    if ( argumentExists(3) )
    {
        int32_t tmp = oodGlobalID(context, helpID, 3, false);
        if ( tmp == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        dwHelpID = tmp;
    }

    if ( argumentOmitted(4) || count == 0 )
    {
        count = DEFAULT_MENUITEM_COUNT;
    }

    if ( argumentExists(6) )
    {
        // We make sure here, that attachToDlg() later will not fail.
        if ( ! validAttachTo(context, attachTo, 6) )
        {
            goto done_out;
        }
    }

    if ( ! cMenu->initTemplate(count, dwHelpID) )
    {
        goto done_out;
    }

    // If wID == -1 and id was not ommitted, then id could be a string menu
    // name.  Or, it could be a bad symbolic ID.  If it is a bad symbol, then
    // load() will raise an exception, so that's ok.
    RexxObjectPtr menuName = (cMenu->wID == -1 && ! idOmitted) ? _id : context->NullString();

    RexxMethodContext *c = context;
    RexxArrayObject args = context->ArrayOfFour(rcFile, c->Int32(cMenu->wID), context->Logical(connect),
                                                context->UnsignedInt32(count));
    context->ArrayAppend(args, idOmitted ? TheTrueObj : TheFalseObj);
    context->ArrayAppend(args, menuName);

    // Note well: the load() method makes repeated calls to addItem(),
    // addPopup(), etc. Each call resets the internal CppMenu context.  When we
    // return, the internal cMenu context will no longer be this context.  It
    // *must* be reset.

    context->SendMessage(self, "LOAD", args);
    cMenu->setContext(context, TheNilObj);

    if ( context->CheckCondition() )
    {
        cMenu->deleteTemplate();
        cMenu->noTempHelpID();
        goto done_out;
    }

    if ( ! cMenu->finishTemplate() )
    {
        goto done_out;
    }

    if ( argumentExists(6) )
    {
        cMenu->attachToDlg(attachTo, 0);
    }

done_out:
    return NULLOBJECT;
}


/**
 *  Methods for the .UserMenuBar class.
 */
#define USERMENUBAR_CLASS       "UserMenuBar"


/** UserMenuBar::init()
 *
 *  Initializes a .UserMenuBar object.  The underlying menu object is created as
 *  a template in memory by the Rexx programmer using methods of the
 *  .MenuTemplate class. The menu is not valid until the programmer invokes the
 *  complete() method which tramsforms the in memory template to an actual menu.
 *
 *
 *  @param  id           [optional]  The resource id of / for the menu.  Can be
 *                       -1 to indicate no ID.  The default is -1.
 *
 *  @param  helpID       [optional]  Context help ID for the menu.  The default
 *                       is 0 indicating no help ID.
 *
 *  @param  count        [optional]  The count of menu items.  This determines
 *                       how much storage is allocated for the menu template.
 *                       This is just a rough estimate, a generous amount of
 *                       storage is allocated for each menu item.  In almost all
 *                       cases more menu items can be added than specified here.
 *                       The default is 100.  If more items are added than there
 *                       is storage for, a condition is raised.  If that
 *                       happens, the count needs to be increased.
 *
 *  @param  autoConnect  [optional]  Turn on autoConnection if true.  Default is
 *                       false.
 *
 *  @param  mName        [optional]  If autoConnect, set autoConnectionMethod to
 *                       this method.  If omitted autoConectionMethod is left
 *                       alone.
 *
 *  @return  No return.
 *
 *  @note  Raises exceptions for all detected problems.  The menu is not
 *         actually created until complete() is invoked.
 *
 *         To use symbolic IDs, the global .ConstDir must be enabled.
 *
 *  @remarks  Note that when using the MENUEX_TEMPLATE_HEADER you are supposed
 *            to be able to set the help ID for the menu, but this does not seem
 *            to work.  So, for this class we save the help ID in CppMenu and
 *            then set it when the complete method is called.
 */
RexxMethod6(RexxObjectPtr, userMenu_init, OPTIONAL_RexxObjectPtr, _id, OPTIONAL_RexxObjectPtr, helpID,
            OPTIONAL_uint32_t, count, OPTIONAL_logical_t, autoConnect, OPTIONAL_CSTRING, mName, OSELF, self)
{
    CppMenu *cMenu = new CppMenu(self, UserMenuBar, context);
    RexxPointerObject cMenuPtr = context->NewPointer(cMenu);
    context->SendMessage1(self, "MENUINIT", cMenuPtr);

    if ( argumentExists(1) )
    {
        int32_t id = oodGlobalID(context, _id, 1, false);
        if ( id == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        cMenu->setMenuID(id);
    }

    DWORD dwHelpID = 0;
    if ( argumentExists(2) )
    {
        int32_t tmp = oodGlobalID(context, helpID, 2, false);
        if ( tmp == OOD_ID_EXCEPTION )
        {
            goto done_out;
        }
        dwHelpID = tmp;
    }

    if ( argumentOmitted(3) || count == 0 )
    {
        count = DEFAULT_MENUITEM_COUNT;
    }

    if ( ! cMenu->initTemplate(count, dwHelpID) )
    {
        goto done_out;
    }

    if ( autoConnect )
    {
        if ( ! cMenu->setAutoConnection(TRUE, mName) )
        {
            outOfMemoryException(context->threadContext);
        }
    }

done_out:
    return NULLOBJECT;
}

/** UserMenuBar::complete()
 *
 *
 */
RexxMethod1(logical_t, userMenu_complete, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    if ( ! cMenu->finishTemplate() )
    {
        return FALSE;
    }
    return TRUE;
}


/** Menu::test()
 *
 *  Used as a convenient way to test code.
 *
 */
RexxMethod1(logical_t, menu_test, CSELF, cMenuPtr)
{
    CppMenu *cMenu = (CppMenu *)cMenuPtr;
    cMenu->setContext(context, TheFalseObj);

    CppMenu *tmpMenu = new CppMenu(cMenu->getSelf(), BinaryMenuBar, context);

    cMenu->test(tmpMenu);
    return 0;
}

void CppMenu::test(CppMenu *other)
{
    printf("CppMenu::test() self=%p other=%p\n", this, other);
    printf("Access protected field? isFinal? %d\n", other->isFinal);
}


static UINT checkCommonTypeOpts(const char *opts, UINT type)
{
    if ( StrStrI(opts, "NOTMENUBARBREAK") != NULL )
    {
        type &= ~MFT_MENUBARBREAK;
    }
    else if ( StrStrI(opts, "MENUBARBREAK") != NULL )
    {
        type |= MFT_MENUBARBREAK;
    }

    if ( StrStrI(opts, "NOTMENUBREAK") != NULL )
    {
        type &= ~MFT_MENUBREAK;
    }
    else if ( StrStrI(opts, "MENUBREAK") != NULL )
    {
        type |= MFT_MENUBREAK;
    }

    if ( StrStrI(opts, "NOTRIGHTJUSTIFY") != NULL )
    {
        type &= ~MFT_RIGHTJUSTIFY;
    }
    else if ( StrStrI(opts, "RIGHTJUSTIFY") != NULL )
    {
        type |= MFT_RIGHTJUSTIFY;
    }
    return type;
}

static UINT checkCommonStateOpts(const char *opts, UINT state)
{
    if ( StrStrI(opts, "NOTDEFAULT") != NULL )
    {
        state &= ~MFS_DEFAULT;
    }
    else if ( StrStrI(opts, "DEFAULT") != NULL )
    {
        state |= MFS_DEFAULT;
    }
    if ( StrStrI(opts, "DISABLED") != NULL )
    {
        state |= MFS_DISABLED;
    }
    if ( StrStrI(opts, "GRAYED") != NULL )
    {
        state |= MFS_GRAYED;
    }
    if ( StrStrI(opts, "ENABLED") != NULL )
    {
        state &= ~MFS_DISABLED;
    }
    if ( StrStrI(opts, "UNHILITE") != NULL )
    {
        state &= ~MFS_HILITE;
    }
    else if ( StrStrI(opts, "HILITE") != NULL )
    {
        state |= MFS_HILITE;
    }
    return state;
}

/**
 * Parses an option string to determine a popup menu's type flags.
 *
 * @param opts Keywords signaling the different MFT_* flags.  These are the
 *             valid keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY RIGHTORDER
 *
 *             To remove these types from an existing item, use NOT, i.e.
 *             NOTRIGHTORDER will remvoe the MFT_RIGHTORDER flag from a menu
 *             item.
 *
 * @return The combined MFT_* flags for a popup menu.
 */
static UINT getPopupTypeOpts(const char *opts, UINT type)
{
    type = checkCommonTypeOpts(opts, type);
    if ( StrStrI(opts, "NOTRIGHTORDER") != NULL )
    {
        type &= ~MFT_RIGHTORDER;
    }
    else if ( StrStrI(opts, "RIGHTORDER") != NULL )
    {
        type |= MFT_RIGHTORDER;
    }
    return type;
}

/**
 * Parses an option string to determine a popup menu's state flags.
 *
 * @param opts Keywords signaling the different MFS_* flags.  These are the
 *             valid keyworkds: DEFAULT NOTDEFAULT DISABLED GRAYED ENABLED
 *             HILITE UNHILITE
 *
 * @return The combined MFS_* flags for a popup menu.
 *
 * Note that with extended menus disabled and grared are the same thing.
 */
static UINT getPopupStateOpts(const char *opts, UINT state)
{
    state = checkCommonStateOpts(opts, state);
    return state;
}

/**
 * Parses an option string to determine a menu item's type flags.
 *
 * @param opts Keywords signaling the different MFT_* flags.  These are the
 *             valid keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY RADIO
 *
 *             To remove these types from an existing item, use NOT, i.e.
 *             NOTRADIO will remvoe the MFT_RADIOCHECK flag from a menu item.
 *
 * @return The combined MFT_* flags for a menu item.
 *
 * Note that RIGHTJUSTIFY is only valid in a menu bar.  Normally menu bars only
 * contain submenus, but menu items are perfectly valid in a menu bar.  If the
 * right justify flag is used in a submenu, it has no effect.
 */
static UINT getItemTypeOpts(const char *opts, UINT type)
{
    type = checkCommonTypeOpts(opts, type);
    if ( StrStrI(opts, "NOTRADIO") != NULL )
    {
        type &= ~MFT_RADIOCHECK;
    }
    else if ( StrStrI(opts, "RADIO") != NULL )
    {
        type |= MFT_RADIOCHECK;
    }
    return type;
}

/**
 * Parses an option string to determine a separtor's type flags.
 *
 * @param opts Keywords signaling the different MFT_* flags.  These are the
 *             valid keywords: MENUBARBREAK MENUBREAK RIGHTJUSTIFY
 *
 *             To remove these types from an existing separator, use NOT, i.e.
 *             NOTMENUBARBREAK will remove the MFT_MENUBARBREAK flag from a menu
 *             separator.
 *
 * @return The combined MFT_* flags for a menu separtor.
 */
static UINT getSeparatorTypeOpts(const char *opts, UINT type)
{
    if ( StrStrI(opts, "NOTMENUBARBREAK") != NULL )
    {
        type &= ~MFT_MENUBARBREAK;
    }
    else if ( StrStrI(opts, "MENUBARBREAK") != NULL )
    {
        type |= MFT_MENUBARBREAK;
    }

    if ( StrStrI(opts, "NOTMENUBREAK") != NULL )
    {
        type &= ~MFT_MENUBREAK;
    }
    else if ( StrStrI(opts, "MENUBREAK") != NULL )
    {
        type |= MFT_MENUBREAK;
    }

    if ( StrStrI(opts, "NOTRIGHTJUSTIFY") != NULL )
    {
        type &= ~MFT_RIGHTJUSTIFY;
    }
    else if ( StrStrI(opts, "RIGHTJUSTIFY") != NULL )
    {
        type |= MFT_RIGHTJUSTIFY;
    }
    return type;
}

/**
 * Parses an option string to determine a menu item's state flags.
 *
 * @param opts The valid keywords are DEFAULT NOTDEFAULT DISABLED GRAYED ENABLED
 *             UNCHECKED CHECKED HILITE UNHILITE.
 * @param state The parsed state from the options is combined with this state,
 *              pesumably the current state.  Use 0 to get the exact state
 *              specified by the opts string.
 *
 * @return UINT
 *
 * Note that with extended menus grayed and disabled are the same thing.
 */
static UINT getItemStateOpts(const char *opts, UINT state)
{
    state = checkCommonStateOpts(opts, state);

    if ( StrStrI(opts, "UNCHECKED") != NULL )
    {
        state &= ~MFS_CHECKED;
    }
    else if ( StrStrI(opts, "CHECKED") != NULL )
    {
        state |= MFS_CHECKED;
    }
    return state;
}


static UINT getTrackFlags(const char *opt)
{
    uint32_t flag = 0;

    if ( StrStrI(opt, "LEFT") != NULL )
    {
        flag = TPM_LEFTALIGN;
    }
    else if ( StrStrI(opt, "HCENTER") != NULL )
    {
        flag = TPM_CENTERALIGN;
    }
    else
    {
        flag = TPM_RIGHTALIGN;
    }

    if ( StrStrI(opt, "TOP") != NULL )
    {
        flag |= TPM_TOPALIGN;
    }
    else if ( StrStrI(opt, "VCENTER") != NULL )
    {
        flag |= TPM_VCENTERALIGN;
    }
    else
    {
        flag |= TPM_BOTTOMALIGN;
    }

    if ( StrStrI(opt, "HORNEGANIMATION") != NULL )
    {
        flag |= TPM_HORNEGANIMATION;
    }
    if ( StrStrI(opt, "HORPOSANIMATION") != NULL )
    {
        flag |= TPM_HORPOSANIMATION;
    }
    if ( StrStrI(opt, "NOANIMATION") != NULL )
    {
        flag |= TPM_NOANIMATION;
    }
    if ( StrStrI(opt, "VERNEGANIMATION") != NULL )
    {
        flag |= TPM_VERNEGANIMATION;
    }
    if ( StrStrI(opt, "VERPOSANIMATION") != NULL )
    {
        flag |= TPM_VERPOSANIMATION;
    }
    if ( StrStrI(opt, "HORIZONTAL") != NULL )
    {
        flag |= TPM_HORIZONTAL;
    }
    if ( StrStrI(opt, "VERTICAL") != NULL )
    {
        flag |= TPM_VERTICAL;
    }
    if ( StrStrI(opt, "RECURSE") != NULL )
    {
        flag |= TPM_RECURSE;
    }
    if ( ComCtl32Version >= COMCTL32_6_0 )
    {
        if ( StrStrI(opt, "LAYOUTRTL") != NULL )
        {
            flag |= TPM_LAYOUTRTL;
        }
    }

    return flag;
}

