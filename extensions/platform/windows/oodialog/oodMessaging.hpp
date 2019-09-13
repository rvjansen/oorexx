/*----------------------------------------------------------------------------*/;
/*                                                                            */;
/* Copyright (c) 2009-2014 Rexx Language Association. All rights reserved.    */;
/*                                                                            */;
/* This program and the accompanying materials are made available under       */;
/* the terms of the Common Public License v1.0 which accompanies this         */;
/* distribution. A copy is also available at the following address:           */;
/* http://www.oorexx.org/license.html                                         */;
/*                                                                            */;
/* Redistribution and use in source and binary forms, with or                 */;
/* without modification, are permitted provided that the following            */;
/* conditions are met:                                                        */;
/*                                                                            */;
/* Redistributions of source code must retain the above copyright             */;
/* notice, this list of conditions and the following disclaimer.              */;
/* Redistributions in binary form must reproduce the above copyright          */;
/* notice, this list of conditions and the following disclaimer in            */;
/* the documentation and/or other materials provided with the distribution.   */;
/*                                                                            */;
/* Neither the name of Rexx Language Association nor the names                */;
/* of its contributors may be used to endorse or promote products             */;
/* derived from this software without specific prior written permission.      */;
/*                                                                            */;
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */;
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */;
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */;
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */;
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */;
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */;
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */;
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */;
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */;
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */;
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */;
/*                                                                            */;
/*----------------------------------------------------------------------------*/;

#ifndef oodMessaging_Included
#define oodMessaging_Included


// Enum for errors during key press processing
typedef enum
{
    noErr         = 0,
    nameErr       = 1,  // The method name was too long, or the empty string.
    winAPIErr     = 2,  // A Windows API failure.
    memoryErr     = 5,  // Memory allocation error.
    maxMethodsErr = 6,  // No room left in the method table.
    dupMethodErr  = 7,  // The method name provided already exists in the table, so nothing was done.
    badFilterErr  = 8,  // The filter provided was not a filter, so it was ignored.
    keyMapErr     = 9   // Some or all of the keys did not get mapped.
} keyPressErr_t;

extern BOOL    APIENTRY RexxSetProcessMessages(BOOL onoff);
extern LRESULT CALLBACK RexxDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK RexxTabOwnerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT CALLBACK RexxChildDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern bool parseWinMessageFilter(RexxMethodContext *context, pWinMessageFilter pwmf);
extern BOOL endDialogPremature(pCPlainBaseDialog, HWND, DlgProcErrType);

extern LRESULT       paletteMessage(pCPlainBaseDialog, HWND, UINT, WPARAM, LPARAM);
extern LRESULT       handleWmCommand(pCPlainBaseDialog pcpbd, HWND hDlg, WPARAM wParam, LPARAM lParam, bool isNestedDlg);
extern LRESULT       handleWmUser(pCPlainBaseDialog pcpbd, HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam, bool isNestedDlg);
extern MsgReplyType  searchMessageTables(ULONG message, WPARAM param, LPARAM lparam, pCPlainBaseDialog);
extern bool          initCommandMessagesTable(RexxMethodContext *c, pCEventNotification pcen, pCPlainBaseDialog pcpbd);
extern bool          initEventNotification(RexxMethodContext *, pCPlainBaseDialog, RexxObjectPtr, pCEventNotification *);
extern bool          addCommandMessage(pCEventNotification, RexxMethodContext *, WPARAM, ULONG_PTR, LPARAM, ULONG_PTR, CSTRING, uint32_t);
extern bool          addNotifyMessage(pCEventNotification, RexxMethodContext *, WPARAM, ULONG_PTR, LPARAM, ULONG_PTR, CSTRING, uint32_t);
extern bool          addMiscMessage(pCEventNotification, RexxMethodContext *, uint32_t, uint32_t, WPARAM, ULONG_PTR, LPARAM, ULONG_PTR, CSTRING, uint32_t);
extern RexxObjectPtr getToolIDFromLParam(RexxThreadContext *c, LPARAM lParam);

// Shared functions for keyboard hooks, key press and key event subclassing.
extern void            removeKBHook(pCEventNotification);
extern keyPressErr_t   setKeyPressData(KEYPRESSDATA *, CSTRING, CSTRING, CSTRING);
extern void            processKeyPress(pSubClassData, WPARAM, LPARAM);
extern void            freeKeyPressData(pSubClassData);
extern uint32_t        seekKeyPressMethod(KEYPRESSDATA *, CSTRING);
extern void            removeKeyPressMethod(KEYPRESSDATA *, uint32_t);
extern RexxArrayObject getKeyEventRexxArgs(RexxThreadContext *c, WPARAM wParam, bool isExtended, RexxObjectPtr rexxControl);
extern void            releaseKeyEventRexxArgs(RexxThreadContext *c, RexxArrayObject args);

// Shared event processing function
extern MsgReplyType  genericInvoke(pCPlainBaseDialog pcpbd, CSTRING method, RexxArrayObject args, uint32_t tag);
extern bool          invokeDirect(RexxThreadContext *c, pCPlainBaseDialog pcpbd, CSTRING methodName, RexxArrayObject args);
extern bool          invokeSync(RexxThreadContext *c, pCPlainBaseDialog pcpbd, CSTRING methodName, RexxArrayObject args);
extern MsgReplyType  invokeDispatch(RexxThreadContext *c, pCPlainBaseDialog pcpbd, CSTRING methodName, RexxArrayObject args);
extern bool          msgReplyIsGood(RexxThreadContext *c, pCPlainBaseDialog pcpbd, RexxObjectPtr reply, CSTRING methodName, bool clear);
extern RexxObjectPtr requiredBooleanReply(RexxThreadContext *c, pCPlainBaseDialog pcpbd, RexxObjectPtr reply, CSTRING method, bool clear);
extern MsgReplyType  genericReleasedCapture(pCPlainBaseDialog pcpbd, CSTRING methodName, uint32_t tag, LPARAM lParam, oodControl_t type);
extern int32_t       keyword2ncHitTestt(CSTRING keyword);
extern CSTRING       ncHitTest2string(WPARAM hit);
extern RexxObjectPtr ncHitTest2string(RexxThreadContext *c, WPARAM hit);

// Process menu event notifications.  Defined in oodMenu.cpp
extern MsgReplyType processMenuMsg(pCPlainBaseDialog pcpbd, uint32_t wmMsg, WPARAM wParam, LPARAM lParam, CSTRING method, uint32_t tag);

// List-view functions.  Defined in oodListView.cpp:
extern MsgReplyType lvnBeginDrag(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag, uint32_t code);
extern MsgReplyType lvnBeginEndScroll(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag, uint32_t code);
extern MsgReplyType lvnBeginLabelEdit(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag);
extern MsgReplyType lvnColumnClick(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag);
extern MsgReplyType lvnEndLabelEdit(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag);
extern MsgReplyType lvnGetInfoTip(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag);
extern MsgReplyType lvnItemChanged(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag);
extern MsgReplyType lvnKeyDown(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag);
extern MsgReplyType lvnNmClick(pCPlainBaseDialog pcpbd, LPARAM lParam, CSTRING methodName, uint32_t tag, uint32_t code);

// ReBar functions.  Defined in oodReBar.cpp:
extern MsgReplyType rbnNcHitTest(pCPlainBaseDialog pcpbd, CSTRING methodName, uint32_t tag, LPARAM lParam);
extern MsgReplyType rbnAutobreak(pCPlainBaseDialog pcpbd, CSTRING methodName, uint32_t tag, LPARAM lParam);

// StatusBar functions.  Defined in oodStatusBar.cpp:
extern MsgReplyType sbnSimpleModeChange(pCPlainBaseDialog pcpbd, CSTRING methodName, uint32_t tag, LPARAM lParam);
extern bool         isSimple(HWND hwnd);

// ToolBar functions.  Defined in oodToolBar.cpp:
extern MsgReplyType tbnDeletingButton(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType tbnGetButtonInfo(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType tbnInitCustomize(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType tbnQuery(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd, uint32_t code);
extern MsgReplyType tbnSimple(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);

// Tree-view notification processing functions.  Defined in oodTreeView.cpp:
extern MsgReplyType  tvnBeginDrag(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd, uint32_t code);
extern MsgReplyType  tvnBeginLabelEdit(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType  tvnDeleteItem(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType  tvnEndLabelEdit(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType  tvnGetInfoTip(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType  tvnItemExpand(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd, uint32_t code);
extern MsgReplyType  tvnKeyDown(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd);
extern MsgReplyType  tvnSelChange(RexxThreadContext *c, CSTRING methodName, uint32_t tag, LPARAM lParam, pCPlainBaseDialog pcpbd, uint32_t code);

/**
 * Releases the local reference on a Rexx object if the objet is not NULL
 */
inline void safeLocalRelease(RexxThreadContext *c, RexxObjectPtr o)
{
    if ( o != NULLOBJECT )
    {
        c->ReleaseLocalReference(o);
    }
}

inline RexxObjectPtr notifyCode2rexxArg(RexxThreadContext *c, LPARAM lParam)
{
    return c->UnsignedInt32(((NMHDR *)lParam)->code);
}

inline RexxObjectPtr idFrom2rexxArg(RexxThreadContext *c, LPARAM lParam)
{
    return c->Uintptr(((NMHDR *)lParam)->idFrom);
}

inline RexxObjectPtr hwndFrom2rexxArg(RexxThreadContext *c, LPARAM lParam)
{
    return pointer2string(c, ((NMHDR *)lParam)->hwndFrom);
}

inline RexxObjectPtr controlFrom2rexxArg(pCPlainBaseDialog pcpbd, LPARAM lParam, oodControl_t ctrl)
{
    return createControlFromHwnd(pcpbd->dlgProcContext, pcpbd, ((NMHDR *)lParam)->hwndFrom, ctrl, true);
}

#endif
