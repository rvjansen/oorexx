;
; ooRexx Install Script, based on Modern Example Script Written by Joost Verburg
;
; This script requires 2 plugins added to your NISI installation.  They need to
; be put into ${NSISDIR}\Plugins directory:
;   services.dll       -> https://nsis.sourceforge.net/File:Services.zip
;   ooRexxProcess.dll  -> https://sourceforge.net/projects/oorexx/files/  under oorexx-buildutils
;
; Run as:
;  makensis /DVERSION=x.x /DNODOTVER=xx /DSRCDIR=xxx /DBINDIR=xxx /DCPU=xxx oorexx.nsi
;  eg
;  makensis /DVERSION=4.0.0 /DNODOTVER=400 /DSRCDIR=d:\oorexx\oorexx /DBINDIR=d:\oorexx\oorexx\win32rel /DCPU=x64 oorexx.nsi
; Note:
;  oorexx.nsi MUST be in the current directory.

!define LONGNAME       "Open Object Rexx"  ;Long Name (for descriptions)
!define SHORTNAME      "ooRexx"            ;Short name (no slash) of package
!define DISPLAYICON    "$INSTDIR\rexx.exe,0"
!define UNINSTALLER    "uninstall.exe"
!define KEYFILE1       "rexx.dll"
!define KEYFILE2       "rexx.exe"
!define KEYFILE3       "rxapi.exe"

; Default file extensions and ftypes
!define DefRexxExt       ".rex"
!define DefRexxFType     "RexxScript"
!define DefRexxHideExt   ".rexg"
!define DefRexxHideFType "RexxHide"
!define DefRexxPawsExt   ".rexp"
!define DefRexxPawsFType "RexxPaws"

Name "${LONGNAME} ${VERSION}"

!include "MUI2.nsh"
!include "Library.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "admin.nsh"
!include "isnt.nsh"
!include "newpath.nsh"
!include "WriteEnv.nsh"
!include "StrFunc.nsh"
!include "x64.nsh"

; Docs for the string functions say they need to be declared before use:
${StrTok}
${StrCase}
${UnStrTok}

!define MUI_ICON "${SRCDIR}\platform\windows\rexx.ico"
!define MUI_UNICON "${SRCDIR}\platform\windows\install\uninstall.ico"

!define MUI_CUSTOMPAGECOMMANDS
!define MUI_WELCOMEFINISHPAGE_BITMAP "orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "orange-uninstall.bmp"

!define MUI_LICENSEPAGE
!define MUI_COMPONENTSPAGE
!define MUI_DIRECTORYPAGE

;---------- Finish page set up --------------------------
!define MUI_FINISHPAGE
!define MUI_FINISHPAGE_NOAUTOCLOSE

; Note that the finish page run function is used here to instead
; create a Desktop icon.  The 'run_function' is set to our create
; Desktop function, where we can do whatever we want.
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_RUN_TEXT "Create ${LONGNAME} Desktop Shortcut"
!define MUI_FINISHPAGE_RUN_FUNCTION CreateDesktopShortcut

!define MUI_FINISHPAGE_SHOWREADME $INSTDIR\doc\ReleaseNotes.txt
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Show ${LONGNAME} Release Notes"
;!define MUI_FINISHPAGE_SHOWREADME_FUNCTION SomeFunctionToBeCreated

!define MUI_FINISHPAGE_LINK "Getting started with Windows ${LONGNAME}"
!define MUI_FINISHPAGE_LINK_LOCATION "https://www.rexxla.org/rexxlang/rexxtut.html"

!define MUI_ABORTWARNING

!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

!define MUI_CUSTOMFUNCTION_UNABORT un.onCancelButton

!define UninstLog "uninstall.log"
Var UninstLog

!define SHACF_FILESYSTEM 1  ; For SHAutoComplete()

!macro AddItem Path
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define AddItem "!insertmacro AddItem"

!macro File FilePath FileName
 FileWrite $UninstLog "$OUTDIR\${FileName}$\r$\n"
 File "${FilePath}${FileName}"
!macroend
!define File "!insertmacro File"

!macro CreateDirectory Path
 CreateDirectory "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define CreateDirectory "!insertmacro CreateDirectory"

!macro SetOutPath Path
 SetOutPath "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define SetOutPath "!insertmacro SetOutPath"

!macro WriteUninstaller Path
 WriteUninstaller "${Path}"
 FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define WriteUninstaller "!insertmacro WriteUninstaller"

;--------------------------------
;Configuration

  ;General
  OutFile "${SHORTNAME}.${VERSION}-${CPU}.exe"
  ShowInstdetails show
  ShowUninstDetails show
  SetOverwrite on
  SetPluginUnload alwaysoff
  RequestExecutionLevel admin
  InstallDir "$PROGRAMFILES\${SHORTNAME}"

  ;InstType "Defualt"
  ;InstType "Simple"
  ;InstType /COMPONENTSONLYONCUSTOM

;--------------------------------
;Pages
  /* Installer pages */
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "${SRCDIR}\CPLv1.0.txt"
  Page custom Uninstall_Old_ooRexx_page Uninstall_Old_ooRexx_Leave
  Page custom Uninstall_Type_page Uninstall_Type_Leave
  Page custom Ok_Stop_RxAPI_page Ok_Stop_RxAPI_leave

  !define MUI_PAGE_CUSTOMFUNCTION_PRE Components_Page_pre
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW Components_Page_show
  !insertmacro MUI_PAGE_COMPONENTS

  !define MUI_PAGE_CUSTOMFUNCTION_PRE Directory_Page_pre
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW Directory_Page_show
  !insertmacro MUI_PAGE_DIRECTORY

  /* rxapi service stuff no longer applies
  Page custom Rxapi_Options_page Rxapi_Options_leave
  */
  Page custom File_Associations_page File_Associations_leave
  Page custom SendTo_Items_page SendTo_Items_leave
  Page custom Associate_rexx_page Associate_rexx_leave
  Page custom Associate_otherExes_page Associate_otherExes_leave
  Page custom Confirm_page

  !insertmacro MUI_PAGE_INSTFILES

  !define MUI_PAGE_CUSTOMFUNCTION_PRE Finish_Page_pre
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW Finish_Page_show
  !insertmacro MUI_PAGE_FINISH

  /* Uninstaller pages */
  !insertmacro MUI_UNPAGE_WELCOME
  UninstPage custom un.Ok_Stop_RxAPI_page un.Ok_Stop_RxAPI_leave
  UninstPage custom un.Uninstall_By_Log_page un.Uninstall_By_Log_leave
  !define MUI_PAGE_CUSTOMFUNCTION_PRE un.Confirm_Page_pre
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
;--------------------------------
;Language
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Reserved files

;--------------------------------
; Variables
Var IsAdminUser                ; is the installer being run by an admin:           true / false
Var RxapiIsService             ; is rxapi installed as a service:                  true / false
Var RxapiIsRunning             ; is rxapi running:                                 true / false

;
Var RegVal_uninstallString     ; uninstall string (program) found in regsitry
Var RegVal_uninstallLocation   ; location of uninstall program found in registry
Var RegVal_uninstallVersion    ; Version / level of uninstaller program.  This only exists at 410 or greater
Var RegVal_uninstallBitness    ; The uninstaller is 32 or 64 bit.  This only exists at 420 or greater.
Var RegVal_rexxAssociation     ; File association string for rexx.exe     (.ext / ftype - i.e. .rex RexxScript)
Var RegVal_rexxEditor          ; ... and editor file name
Var RegVal_rexxHideAssociation ; File association string for rexxhide.exe (.ext / ftype - i.e. .rexg RexxHide)
Var RegVal_rexxPawsAssociation ; File association string for rexxpaws.exe (.ext / ftype - i.e. .rexp RexxPaws)
Var RegVal_sendTo_rexx         ; Add / don't add Send To Rexx item
Var RegVal_sendTo_rexxHide     ; Add / don't add Send To rexxpaws item
Var RegVal_sendTo_rexxPaws     ; Add / don't add Send To rexxhide item
Var RegVal_desktop_icon        ; Create / don't create a Desktop icon

Var AssociationProgramName     ; Executable being associated  (i.e rexxpaws.exe, rexx.exe, etc..)
Var AssociationText            ; Descriptive text that goes in registry  (i.e. ooRexx Rexx GUI Program)
Var AssociationEditor          ; File path name of editor that goes in registry  (i.e. C:\Windows\system32\NotePad.exe)

Var PreviousVersionInstalled   ; A previous version is installed                   true / false
Var DoUpgrade                  ; try to do an upgrade install                      true / false
Var DoUpgradeQuick             ; don't show options pages with diasabled controls  true / false
Var UpgradeTypeAvailable       ; Level of uninstaller sufficient for upgrade type  true / false

Var KeyFileName                ; Used to contruct full path to key files.
Var CheckRxApiLock             ; Used to skipping checking if rxapi is locked.
Var UserRequestAbort           ; General purpose, set to true if User replies Ok, wanting to abort.

; Dialog variables
Var Dialog
Var Label_One
Var Label_Two

Var Uninstall_Previous_CK
Var Force_Install_CK

Var Do_Upgrade_Type_CK
Var Do_Upgrade_Type_CK_state
Var Do_Upgrade_Quick_CK
Var Do_Upgrade_Quick_CK_state

Var RxAPI_Install_Service_CK
Var RxAPI_Start_CK
Var RxAPIInstallService
Var RxAPIStartService

Var StopRxAPI_CK
Var StopRxAPI_CK_State

Var Use_File_Associations_CK
Var Use_File_Associations_CK_state

Var SendTo_rexx_CK
Var SendTo_rexx_CK_state
Var SendTo_rexxHide_CK
Var SendTo_rexxHide_CK_state
Var SendTo_rexxPaws_CK
Var SendTo_rexxPaws_CK_state

; rexx.exe file associations page controls
Var Rexx_editor_EDIT
Var Rexx_editor_text
Var Rexx_editor_PB
Var Associate_rexx_CK
Var Associate_rexx_CK_state
Var Rexx_ext_EDIT
Var Rexx_ext_text
Var Rexx_ftype_EDIT
Var Rexx_ftype_text

; rexxhide.exe file associations
Var Associate_rexxhide_CK
Var Associate_rexxhide_CK_state
Var RexxHide_ext_EDIT
Var RexxHide_ext_text
Var RexxHide_ftype_EDIT
Var RexxHide_ftype_text

; rexxpaws.exe file associations
Var Associate_rexxpaws_CK
Var Associate_rexxpaws_CK_state
Var RexxPaws_ext_EDIT
Var RexxPaws_ext_text
Var RexxPaws_ftype_EDIT
Var RexxPaws_ftype_text

; Uninstall variables
Var InStopRxapiPage
Var LogFileExists
Var Delete_ooRexx_Tree_CK
Var DeleteWholeTree

;===============================================================================
;Installer Sections
;===============================================================================

;-------------------------------------------------------------------------------
;  Hidden section to open the log file

Section -openlogfile
  CreateDirectory "$INSTDIR"
  Call OpenUninstallLog
SectionEnd

;-------------------------------------------------------------------------------
; Core components

Section "${LONGNAME} Core (required)" SecMain
  SectionIn 1 RO

  DetailPrint "Beginning installation of ooRexx"
  DetailPrint "  NSIS installer with max string length ${NSIS_MAX_STRLEN}"
  DetailPrint ""

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR
  ; Distribution executables...
  ${File} "${BINDIR}\" "ooDialog.com"
  ${File} "${BINDIR}\" "ooDialog.exe"
  ${File} "${BINDIR}\" "rexx.exe"
  ${File} "${BINDIR}\" "rexx.img"
  ${File} "${BINDIR}\" "rexxc.exe"
  ${File} "${BINDIR}\" "rxsubcom.exe"
  ${File} "${BINDIR}\" "rxqueue.exe"
  ${File} "${BINDIR}\" "rxapi.exe"
  ${File} "${BINDIR}\" "rexxhide.exe"
  ${File} "${BINDIR}\" "rexxpaws.exe"
  ; Distribution DLLs...
  ${File} "${BINDIR}\" "rexx.dll"
  ${File} "${BINDIR}\" "rexxapi.dll"
  ${File} "${BINDIR}\" "rexxutil.dll"
  ${File} "${BINDIR}\" "rxmath.dll"
  ${File} "${BINDIR}\" "rxsock.dll"
  ${File} "${BINDIR}\" "rxregexp.dll"
  ${File} "${BINDIR}\" "rxwinsys.dll"
  ${File} "${BINDIR}\" "oodialog.dll"
  ${File} "${BINDIR}\" "orexxole.dll"
  ${File} "${BINDIR}\" "hostemu.dll"
  ; CLASS files...
  ${File} "${BINDIR}\" "winsystm.cls"
  ${File} "${BINDIR}\" "socket.cls"
  ${File} "${BINDIR}\" "streamsocket.cls"
  ${File} "${BINDIR}\" "mime.cls"
  ${File} "${BINDIR}\" "smtp.cls"
  ${File} "${BINDIR}\" "rxregexp.cls"
  ${File} "${BINDIR}\" "rxftp.cls"
  ${File} "${BINDIR}\" "csvStream.cls"
  ${File} "${BINDIR}\" "orexxole.cls"
  ${File} "${BINDIR}\" "ooDialog.cls"
  ${File} "${BINDIR}\" "oodWin32.cls"
  ${File} "${BINDIR}\" "oodPlain.cls"

  ; rexxtry is technically a sample, but it is heavily used, so add it to
  ; the executables.  The same thing for the GUI version.
  ${File} "${SRCDIR}\samples\" "rexxtry.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\ooRexxTry\" "ooRexxTry.rex"

  ; Other files...
  ${File} "${SRCDIR}\platform\windows\" "rexx.ico"
  ${File} "${SRCDIR}\" "CPLv1.0.txt"

  ; Set output path to the installation directory just in case
  SetOutPath $INSTDIR

  /*  Comment out orxscrpt stuff temporarily
  ; orxscrpt.dll needs to be registered
  !insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "${BINDIR}\orxscrpt.dll" "$INSTDIR\orxscrpt.dll" "$INSTDIR"

  ; Stop rxapi.exe (again!) the registration process starts rxapi.exe.
  ooRexxProcess::killProcess "rxapi.exe"
  */

  ; Add the Start Menu folder and start adding the items.
  ${CreateDirectory} "$SMPROGRAMS\${LONGNAME}"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Try Rexx.lnk" "$INSTDIR\rexx.exe" '"$INSTDIR\rexxtry.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\Try Rexx.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Try Rexx (GUI).lnk" "$INSTDIR\rexx.exe" '"$INSTDIR\ooRexxTry.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\Try Rexx (GUI).lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\Uninstall ${SHORTNAME}.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\Uninstall ${SHORTNAME}.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} LICENSE.lnk" "$INSTDIR\CPLv1.0.txt" "" "$INSTDIR\CPLv1.0.txt" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} LICENSE.lnk"

  WriteINIStr "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Home Page.url" "InternetShortcut" "URL" "https://www.oorexx.org/"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Home Page.url"

  ; If we are doing an upgrade, these settings are all left however they were.
  ${If} $DoUpgrade == 'false'
    ; Maybe create Send To items.
    Call DoSendToItems

    ; Do the file associations, like associate .rex with ooRexx (REXXScript).
    ; DoFileAssociations needs to run before DoEnvVariagles.
    Call DoFileAssociations

    ; Set the environment variables, PATH, REXX_HOME, etc..
    Call DoEnvVariables
  ${EndIf}


  ; Write the uninstall keys.  Note that the uninstaller always deletes these
  ; keys, even if doing an upgrade.  So we need to still write the file
  ; assoication keys.  For an upgrade install, these will be exactly what we
  ; read in on init.
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "InstallLocation" '"$INSTDIR"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "DisplayName" "${LONGNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "DisplayIcon" "${DISPLAYICON}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "HelpLink" "https://www.rexxla.org/support.html"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "URLUpdateInfo" "https://sourceforge.net/project/showfiles.php?group_id=119701"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "URLInfoAbout" "https://www.rexxla.org/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "Publisher" "Rexx Language Association"

  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallString" '"$INSTDIR\${UNINSTALLER}"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UnInstallLocation" "$INSTDIR" ; dont quote it
  WriteRegStr       HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallVersion" "${VERSION}"
  WriteRegStr       HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallBitness" "${CPU}"
  WriteRegDWORD     HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "NoModify" 0x00000001
  WriteRegDWORD     HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "NoRepair" 0x00000001

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxAssociation" $RegVal_rexxAssociation
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxEditor" $RegVal_rexxEditor
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxHideAssociation" $RegVal_rexxHideAssociation
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxPawsAssociation" $RegVal_rexxPawsAssociation

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "SendToRexx" $RegVal_sendTo_rexx
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "SendToRexxHide" $RegVal_sendTo_rexxHide
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "SendToRexxPaws" $RegVal_sendTo_rexxPaws

  ${WriteUninstaller} "$INSTDIR\${UNINSTALLER}"

SectionEnd

;-------------------------------------------------------------------------------
; Sample programs

Section "${LONGNAME} Samples" SecDemo
  DetailPrint "********** Samples **********"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\" "0ReadMe.first"
  ${File} "${SRCDIR}\samples\" "rexxcps.rex"
  ${File} "${SRCDIR}\samples\" "ccreply.rex"
  ${File} "${SRCDIR}\samples\" "complex.rex"
  ${File} "${SRCDIR}\samples\" "factor.rex"
  ${File} "${SRCDIR}\samples\" "greply.rex"
  ${File} "${SRCDIR}\samples\" "guess.rex"
  ${File} "${SRCDIR}\samples\" "ktguard.rex"
  ${File} "${SRCDIR}\samples\" "makestring.rex"
  ${File} "${SRCDIR}\samples\" "month.rex"
  ${File} "${SRCDIR}\samples\" "philfork.rex"
  ${File} "${SRCDIR}\samples\" "pipe.rex"
  ${File} "${SRCDIR}\samples\" "properties.rex"
  ${File} "${SRCDIR}\samples\" "qdate.rex"
  ${File} "${SRCDIR}\samples\" "qtime.rex"
  ${File} "${SRCDIR}\samples\" "scclient.rex"
  ${File} "${SRCDIR}\samples\" "scserver.rex"
  ${File} "${SRCDIR}\samples\" "semcls.rex"
  ${File} "${SRCDIR}\samples\" "sfclient.rex"
  ${File} "${SRCDIR}\samples\" "sfserver.rex"
  ${File} "${SRCDIR}\samples\" "stack.rex"
  ${File} "${SRCDIR}\samples\" "usecomp.rex"
  ${File} "${SRCDIR}\samples\" "usepipe.rex"
  ${File} "${SRCDIR}\samples\windows\rexutils\" "drives.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\misc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\misc\" "fileDrop.empty"
  ${File} "${SRCDIR}\samples\windows\misc\" "fileDrop.input"
  ${File} "${SRCDIR}\samples\windows\misc\" "fileDrop.readMe"
  ${File} "${SRCDIR}\samples\windows\misc\" "fileDrop.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\ole
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\ole\" "ReadMe.first"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\ole\adsi
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\ole\adsi\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\ole\apps
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "MSAccessDemo.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "oleUtils.frm"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp01.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp02.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp03.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp04.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp05.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp06.mwp"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp06.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp07.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp08.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp09.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp10.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp11.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp12.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp13.rex"
  ${File} "${SRCDIR}\samples\windows\ole\apps\" "samp14.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\ole\methinfo
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\ole\methinfo\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\ole\methinfo\" "*.cls"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\ole\wmi
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\ole\wmi\" "*.rex"

;;; Temporarily block out the orxscrpt samples
;;;   ; Set output path to the installation directory.
;;;   SetOutPath $INSTDIR\samples\wsh
;;;   ; Add the files ...
;;;   ${File} "${SRCDIR}\samples\windows\wsh\" "*.rex"
;;;   ${File} "${SRCDIR}\samples\windows\wsh\" "*.htm"
;;;   ${File} "${SRCDIR}\samples\windows\wsh\" "*.wsf"
;;;   ${File} "${SRCDIR}\samples\windows\wsh\" "*.wsc"

  ;
  ; OOdialog samples
  ;
  ${SetOutPath} $INSTDIR\samples\oodialog
  ; Add the files ...
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "oodialog.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\bmp\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\bmp\" "*.ico"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\controls
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\" "*.txt"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\ComboBox
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ComboBox\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ComboBox\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ComboBox\" "*.rex"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\ListBox
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListBox\" "*.rex"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\ListView
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\" "*.txt"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\ListView\rc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\rc\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\rc\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\rc\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\rc\" "*.dll"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\rc\" "res.mak"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\ListView\subitem.editing
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\subitem.editing\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\subitem.editing\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\subitem.editing\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ListView\subitem.editing\" "*.txt"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\ToolTip
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ToolTip\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ToolTip\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ToolTip\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ToolTip\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\ToolTip\" "*.txt"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\TreeView
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\TreeView\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\TreeView\" "*.inp"

  ; Set the installation directory:
  ${SetOutPath} $INSTDIR\samples\oodialog\controls\TreeView\rc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\TreeView\rc\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\TreeView\rc\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\controls\TreeView\rc\" "*.bmp"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\examples
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\examples\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\examples\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\examples\resources
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\examples\resources\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\examples\resources\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\examples\resources\" "*.rc"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\menus
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\menus\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\menus\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\menus\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\menus\" "*.rc"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\mouse
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\mouse\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\mouse\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\mouse\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\mouse\" "*.cur"
  ${File} "${SRCDIR}\samples\windows\oodialog\mouse\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\oleinfo
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\oleinfo\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\oleinfo\" "*.txt"
  ${File} "${SRCDIR}\samples\windows\oodialog\oleinfo\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\oleinfo\" "*.rc"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\ooRexxTry
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\ooRexxTry\" "ooRexxTry.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\ooRexxTry\doc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\ooRexxTry\doc\" "ooRexxTry.pdf"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\propertySheet.tabControls
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\" "oodListViews.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\" "PropertySheetDemo.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\" "TabDemo.rex"
;  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\" "TabOwnerDemo.rex" Don't include in 4.2.0
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\" "ticketWizard.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\" "UserTabDemo.rex"

  ${SetOutPath} $INSTDIR\samples\oodialog\propertySheet.tabControls\rc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\rc\" "oodListViews*"
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\rc\" "PropertySheetDemo*"
  ; ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\rc\" "TabOwnerDemo*"   ; Do not expose this example yet
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\rc\" "ticketWizard*"
  ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\rc\" "UserTabDemo.h"
  ; ${File} "${SRCDIR}\samples\windows\oodialog\propertySheet.tabControls\rc\" "UserTabDemo.rc"  ; Do not included in distribution, maybe in the future.

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\rc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\rc\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\rc\" "*.rc"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\res
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\res\" "*.dll"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\resizableDialogs
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\resizableDialogs\DialogAreaU
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\DialogAreaU\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\DialogAreaU\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\resizableDialogs\ResizingAdmin
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\ResizingAdmin\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\resizableDialogs\ResizingAdmin\rc
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\ResizingAdmin\rc\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\ResizingAdmin\rc\" "*.dll"
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\ResizingAdmin\rc\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\resizableDialogs\ResizingAdmin\rc\" "*.rc"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\simple
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\simple\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\simple\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\simple\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\simple\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\sysinfo
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\sysinfo\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\sysinfo\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\sysinfo\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\source
  ; Add the files ...
  ${File} "${BINDIR}\" "ooDialog.cls"
  ${File} "${BINDIR}\" "oodWin32.cls"
  ${File} "${BINDIR}\" "oodPlain.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "0_READ_ME_FIRST.txt"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "build_ooDialog_cls.rex"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "AnimatedButton.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "BaseDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "ControlDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "DeprecatedClasses.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "DialogControls.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "DialogExtensions.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "DynamicDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "EventNotification.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "ListView.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "Menu.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "PlainBaseDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "RcDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "ResDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "ShellObjects.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "ToolTip.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "TreeView.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "UserDialog.cls"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "UtilityClasses.cls"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\tutorial
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\tutorial\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\tutorial\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\tutorial\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\tutorial\" "*.h"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\" "*.txt"

  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise02
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise02\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise03
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise03\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise04
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise04\Extras
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\Extras\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise04\Extras\DlgData
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\Extras\DlgData\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\Extras\DlgData\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\Extras\DlgData\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise04\Extras\DlgData\res
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise04\Extras\DlgData\res\" "*.dll"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise05
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise05\res
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\res\" "res.mak"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\res\" "ProductView.dll"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\res\" "ProductIcon.bmp"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise05\Support
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise05\Support\" "NumberOnlyEditEx.cls"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Customer
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Customer\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Customer\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Customer\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Customer\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Customer\bmp\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Customer\bmp\" "*.ico"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Extras
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Extras\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Extras\Popups
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Extras\Popups\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Order
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Order\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Order\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Order\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Order\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Order\bmp\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Order\bmp\" "*.ico"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\OrderMgr
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\OrderMgr\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\OrderMgr\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\OrderMgr\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Product
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Product\res
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\res\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\res\" "*.dll"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\res\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Product\res\" "res.mak"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise06\Support
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise06\Support\" "*.cls"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Customer
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Customer\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Customer\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Customer\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Customer\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Customer\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Customer\bmp\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Customer\bmp\" "*.ico"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Extras
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Extras\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Extras\Person
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Extras\Person\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Extras\Person\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Extras\Person\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Extras\Person\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Order
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Order\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Order\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Order\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Order\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Order\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Order\bmp\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Order\bmp\" "*.ico"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\OrderMgr
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\OrderMgr\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\OrderMgr\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\OrderMgr\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Product
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Product\res
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\res\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\res\" "*.dll"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\res\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Product\res\" "res.mak"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise07\Support
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Support\" "*.cls"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Support\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Support\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise07\Support\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Customer
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Customer\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\bmp\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\bmp\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Customer\bmp\" "*.cur"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Extras
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Extras\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Extras\Person
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Extras\Person\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Extras\Person\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Extras\Person\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Extras\Person\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Order
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Order\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Order\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Order\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Order\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Order\bmp
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Order\bmp\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Order\bmp\" "*.bmp"

  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\OrderMgr
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\OrderMgr\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\OrderMgr\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\OrderMgr\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Product
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Product\res
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\res\" "*.ico"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\res\" "*.bmp"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\res\" "*.dll"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Product\res\" "res.mak"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\userGuide\exercises\Exercise08\Support
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Support\" "*.cls"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Support\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Support\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\userGuide\exercises\Exercise08\Support\" "*.rex"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\wav
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\wav\" "*.wav"
  ${File} "${SRCDIR}\samples\windows\oodialog\wav\" "*.txt"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\samples\oodialog\winsystem
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\oodialog\winsystem\" "*.rex"
  ${File} "${SRCDIR}\samples\windows\oodialog\winsystem\" "*.rc"
  ${File} "${SRCDIR}\samples\windows\oodialog\winsystem\" "*.h"
  ${File} "${SRCDIR}\samples\windows\oodialog\winsystem\" "*.frm"

  ;
  ; API samples
  ;
  ${SetOutPath} $INSTDIR\samples\api
  ${File} "${SRCDIR}\samples\windows\api\" "readme.txt"
  ; Set output path to the installation directory for callrxnt.
  ${SetOutPath} $INSTDIR\samples\api\callrxnt
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\api\callrxnt\" "backward.fnc"
  ${File} "${SRCDIR}\samples\windows\api\callrxnt\" "callrxnt.c"
  ${File} "${SRCDIR}\samples\windows\api\callrxnt\" "callrxnt.ico"
  ${File} "${SRCDIR}\samples\windows\api\callrxnt\" "callrxnt.mak"
  ${File} "${SRCDIR}\samples\windows\api\callrxnt\" "callrxnt.exe"

  ; Set output path to the installation directory for callrxwn.
  ${SetOutPath} $INSTDIR\samples\api\callrxwn
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "backward.fnc"
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "callrxwn.c"
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "callrxwn.h"
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "callrxwn.ico"
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "callrxwn.mak"
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "callrxwn.exe"
  ${File} "${SRCDIR}\samples\windows\api\callrxwn\" "callrxwn.rc"

  ; Set output path to the installation directory for rexxexit.
  ${SetOutPath} $INSTDIR\samples\api\rexxexit
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\api\rexxexit\" "rexxexit.c"
  ${File} "${SRCDIR}\samples\windows\api\rexxexit\" "rexxexit.ico"
  ${File} "${SRCDIR}\samples\windows\api\rexxexit\" "rexxexit.mak"
  ${File} "${SRCDIR}\samples\windows\api\rexxexit\" "rexxexit.exe"
  ${File} "${SRCDIR}\samples\windows\api\rexxexit\" "testRexxExit"

  ; Set output path to the installation directory the wpipe examples.
  ${SetOutPath} $INSTDIR\samples\api\wpipe
  ; Add the files ...
  ;${File} "${SRCDIR}\samples\windows\api\wpipe\" "readme.txt"

  ; Set output path to the installation directory for wpipe 1.
  ${SetOutPath} $INSTDIR\samples\api\wpipe1
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\api\wpipe1\" "rexxapi1.c"
  ${File} "${SRCDIR}\samples\windows\api\wpipe1\" "rexxapi1.def"
  ${File} "${SRCDIR}\samples\windows\api\wpipe1\" "apitest1.rex"
  ${File} "${SRCDIR}\samples\windows\api\wpipe1\" "rexxapi1.mak"
  ;${File} "${SRCDIR}\samples\windows\api\wpipe1\" "rexxapi1.dll"
  ${File} "${BINDIR}\" "rexxapi1.dll"

  ; Set output path to the installation directory for wpipe 2.
  ${SetOutPath} $INSTDIR\samples\api\wpipe2
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\api\wpipe2\" "rexxapi2.c"
  ${File} "${SRCDIR}\samples\windows\api\wpipe2\" "rexxapi2.def"
  ${File} "${SRCDIR}\samples\windows\api\wpipe2\" "apitest2.rex"
  ${File} "${SRCDIR}\samples\windows\api\wpipe2\" "rexxapi2.mak"
  ${File} "${BINDIR}\" "rexxapi2.dll"

  ; Set output path to the installation directory for wpipe 3.
  ${SetOutPath} $INSTDIR\samples\api\wpipe3
  ; Add the files ...
  ${File} "${SRCDIR}\samples\windows\api\wpipe3\" "rexxapi3.c"
  ${File} "${SRCDIR}\samples\windows\api\wpipe3\" "rexxapi3.def"
  ${File} "${SRCDIR}\samples\windows\api\wpipe3\" "apitest3.rex"
  ${File} "${SRCDIR}\samples\windows\api\wpipe3\" "rexxapi3.mak"
  ${File} "${BINDIR}\" "rexxapi3.dll"

  ;
  ; C++ API samples
  ;
  ${SetOutPath} $INSTDIR\samples\native.api
  ${File} "${SRCDIR}\samples\native.api\" "ReadMe.txt"
  ; Set output path to the installation directory for callexample.
  ${SetOutPath} $INSTDIR\samples\native.api\call.example
  ; Add the files ...
  ${File} "${SRCDIR}\samples\native.api\call.example\" "backward.fnc"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "HelloWorld.rex"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "Makefile.windows"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "ReadMe.txt"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "runRexxProgram.cpp"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "stackOverflow.cpp"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "tooRecursiveTrapped.rex"
  ${File} "${SRCDIR}\samples\native.api\call.example\" "tooRecursiveUnhandled.rex"

  ;
  ; Create start menu shortcuts for some of the example programs.
  ;

  ; $OUTDIR is used as the working directory (the start in directory.)  Change
  ; $OUTDIR by using SetOutPath.  Don't use the ${SetOutPath} macro which adds
  ; the directory to the uninstall log.  That is not relevant here.

  ${CreateDirectory} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples"

  SetOutPath $INSTDIR\samples
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\RexxCPS.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\rexxcps.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\RexxCPS.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Quick Date.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\qdate.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Quick Date.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Quick Time.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\qtime.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Quick Time.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Display Drive Info.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\drives.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Display Drive Info.lnk"

  SetOutPath $INSTDIR\samples\ole\apps
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\MS Access.lnk" "$INSTDIR\rexxpaws.exe" '"$INSTDIR\samples\ole\apps\MSAccessDemo.rex"' "$INSTDIR\rexx.exe"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\MS Access.lnk"

  SetOutPath $INSTDIR\samples\oodialog\winsystem
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Display Window Tree.lnk" "$INSTDIR\ooDialog.exe" '"$INSTDIR\samples\oodialog\winsystem\displayWindowTree.rex"' "$INSTDIR\rexx.exe"
  ${AddItem}        "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Display Window Tree.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Windows Manager.lnk" "$INSTDIR\ooDialog.exe" '"$INSTDIR\samples\oodialog\winsystem\usewmgr.rex"' "$INSTDIR\rexx.exe"
  ${AddItem}         "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\Windows Manager.lnk"

  ${CreateDirectory} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog"

  SetOutPath $INSTDIR\samples\oodialog\Controls\ComboBox
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\Combo Box Types.lnk" "$INSTDIR\ooDialog.exe" '"$INSTDIR\samples\oodialog\Controls\ComboBox\comboBoxTypes.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  ${AddItem}         "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\Combo Box Types.lnk"

  SetOutPath $INSTDIR\samples\oodialog\Controls\ToolTip
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\Custom Position Tool Tips.lnk" "$INSTDIR\ooDialog.exe" '"$INSTDIR\samples\oodialog\Controls\ToolTip\customPositionToolTip.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  ${AddItem}         "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\Custom Position Tool Tips.lnk"

  SetOutPath $INSTDIR\samples\oodialog\Controls\ListView
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\List-view Views.lnk" "$INSTDIR\ooDialog.exe" '"$INSTDIR\samples\oodialog\Controls\ListView\columnIcons.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  ${AddItem}         "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\List-view Views.lnk"

  SetOutPath $INSTDIR\samples\oodialog
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\Samples.lnk" "$INSTDIR\ooDialog.exe" '"$INSTDIR\samples\oodialog\sample.rex"' "$INSTDIR\samples\oodialog\oodialog.ico"
  ${AddItem}        "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\ooDialog\Samples.lnk"

  ${CreateDirectory} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API"

  SetOutPath $INSTDIR\samples\api\callrxnt
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx in a Console.lnk" "$INSTDIR\samples\api\callrxnt\callrxnt.exe" "" "$INSTDIR\samples\api\callrxnt\callrxnt.ico"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx in a Console.lnk"

  SetOutPath $INSTDIR\samples\api\callrxwn
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx in a Window.lnk" "$INSTDIR\samples\api\callrxwn\callrxwn.exe" "" "$INSTDIR\samples\api\callrxwn\callrxwn.ico"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx in a Window.lnk"

  SetOutPath $INSTDIR\samples\api\rexxexit
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx with Exits.lnk" "$INSTDIR\samples\api\rexxexit\rexxexit.exe" 'testRexxExit "189 8"' "$INSTDIR\samples\api\rexxexit\rexxexit.ico"
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Samples\API\Call ooRexx with Exits.lnk"

SectionEnd

;------------------------------------------------------------------------
; Development tools

Section "${LONGNAME} Development Kit" SecDev
  DetailPrint "********** Development Kit **********"
  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\api
  ; Add the files ...
  ${File} "${BINDIR}\" "rexx.lib"
  ${File} "${BINDIR}\" "rexxapi.lib"
  ${File} "${SRCDIR}\api\" "oorexxapi.h"
  ${File} "${SRCDIR}\api\" "rexx.h"
  ${File} "${SRCDIR}\api\" "oorexxerrors.h"
  ${File} "${SRCDIR}\api\" "rexxapidefs.h"
  ${File} "${SRCDIR}\api\platform\windows\" "rexxapitypes.h"
  ${File} "${SRCDIR}\api\platform\windows\" "rexxplatformapis.h"
  ${File} "${SRCDIR}\api\platform\windows\" "rexxplatformdefs.h"

SectionEnd

;------------------------------------------------------------------------
; Documentation

Section "${LONGNAME} Documentation" SecDoc
  DetailPrint "********** Documentation **********"

  ; Set output path to the installation directory.
  ${SetOutPath} $INSTDIR\doc

  ; readmes
  ${File} "${DOCDIR}\" "readme.pdf"

  ; We can't use the ${File} macro here because we need to change the file name.
  ; That means we also have to add the item manually to the uninstall log.
  File /oname=CHANGES.txt "${SRCDIR}\CHANGES"
  File /oname=ReleaseNotes.txt "${SRCDIR}\ReleaseNotes"
  ${AddItem} $INSTDIR\doc\CHANGES.txt
  ${AddItem} $INSTDIR\doc\ReleaseNotes.txt

  ${File} "${DOCDIR}\" "rexxpg.pdf"
  ${File} "${DOCDIR}\" "rexxref.pdf"
  ${File} "${DOCDIR}\" "rxmath.pdf"
  ${File} "${DOCDIR}\" "rxsock.pdf"
  ${File} "${DOCDIR}\" "rxftp.pdf"
  ${File} "${DOCDIR}\" "oodialog.pdf"
  ${File} "${SRCDIR}\extensions\platform\windows\oodialog\" "ooDialog_ReleaseNotes.txt"
  ${File} "${DOCDIR}\" "oodguide.pdf"
  ${File} "${DOCDIR}\" "rexxextensions.pdf"
  ${File} "${DOCDIR}\" "winextensions.pdf"
  ${File} "${SRCDIR}\samples\windows\oodialog\ooRexxTry\doc\" "ooRexxTry.pdf"

  ; Create start menu shortcuts
  ${CreateDirectory} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx README.lnk" "$INSTDIR\doc\readme.pdf" "" "$INSTDIR\doc\readme.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx README.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx CHANGES.lnk" "$INSTDIR\doc\CHANGES.txt" "" "$INSTDIR\doc\CHANGES.txt" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx CHANGES.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx ReleaseNotes.lnk" "$INSTDIR\doc\ReleaseNotes.txt" "" "$INSTDIR\doc\ReleaseNotes.txt" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx ReleaseNotes.lnk"

  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Reference.lnk" "$INSTDIR\doc\rexxref.pdf" "" "$INSTDIR\doc\rexxref.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Programming Guide.lnk" "$INSTDIR\doc\rexxpg.pdf" "" "$INSTDIR\doc\rexxpg.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Programming Guide.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Mathematical Functions Reference.lnk" "$INSTDIR\doc\rxmath.pdf" "" "$INSTDIR\doc\rxmath.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Mathematical Functions Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx TCP-IP Sockets Functions Reference.lnk" "$INSTDIR\doc\rxsock.pdf" "" "$INSTDIR\doc\rxsock.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx TCP-IP Sockets Functions Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx rxFTP Class Reference.lnk" "$INSTDIR\doc\rxftp.pdf" "" "$INSTDIR\doc\rxftp.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx rxFTP Class Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooDialog Reference.lnk" "$INSTDIR\doc\oodialog.pdf" "" "$INSTDIR\doc\oodialog.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooDialog Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooDialog Release Notes.lnk" "$INSTDIR\doc\ooDialog_ReleaseNotes.txt" "" "$INSTDIR\doc\ooDialog_ReleaseNotes.txt" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooDialog Release Notes.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooDialog User Guide.lnk" "$INSTDIR\doc\oodguide.pdf" "" "$INSTDIR\doc\oodguide.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooDialog User Guide.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Rexx Extensions Reference.lnk" "$INSTDIR\doc\rexxextensions.pdf" "" "$INSTDIR\doc\rexxextensions.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Rexx Extensions Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Unix Extensions Reference.lnk" "$INSTDIR\doc\unixextensions.pdf" "" "$INSTDIR\doc\unixextensions.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Unix Extensions Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Windows Extensions Reference.lnk" "$INSTDIR\doc\winextensions.pdf" "" "$INSTDIR\doc\winextensions.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexx Windows Extensions Reference.lnk"
  CreateShortCut "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexxTry Reference.lnk" "$INSTDIR\doc\ooRexxTry.pdf" "" "$INSTDIR\doc\ooRexxTry.pdf" 0
  ${AddItem} "$SMPROGRAMS\${LONGNAME}\${SHORTNAME} Documentation\ooRexxTry Reference.lnk"

SectionEnd

;-------------------------------------------------------------------------------
;  Hidden section to close the log file

Section -closelogfile
 Call CloseUninstallLog
SectionEnd

;===============================================================================
;Installer Functions
;===============================================================================

/** .onInit()  Call back function
 *
 * Called by the installer before any page is shown.  We use it to check for a
 * previously installed ooRexx and to set execution variables.
 *
 * Note that if we end up doing an upgrade type of install,
 * RegVal_uninstallLocation needs to be copied to the INSTDIR variable so that
 * we install in the same place as previous.
 */
Function .onInit

  Call CheckStrLen
  ${If} $UserRequestAbort == 'true'
    abort
  ${EndIf}

  ${If} ${CPU} == "x86_64"
    ${If} ${RunningX64}
      strcpy $INSTDIR "$PROGRAMFILES64\${SHORTNAME}"
    ${else}
      ; If we install 64 bit binaries on a 32 bit OS, things won't work.
      MessageBox MB_OK \
        "The installer detected that this operaring system is 32 bit.  This$\n\
        installer contains 64-bit binaries.  These binaries will not$\n\
        work on a 32-bit operating.$\n$\n\
        The installer will quit.  Please use the 32-bit ooRexx installer$\n\
        to install on this operating system." \
        /SD IDOK
        Abort
    ${endIf}
  ${EndIf}

  ;
  ; Install as All Users if an admin
  ;
  Call IsUserAdmin
  Pop $IsAdminUser
  ${If} $IsAdminUser == "true"
    SetShellVarContext all
    StrCpy $RxAPIInstallService ${BST_CHECKED}
    StrCpy $RxAPIStartService ${BST_CHECKED}
  ${else}
    StrCpy $RxAPIInstallService ${BST_UNCHECKED}
    StrCpy $RxAPIStartService ${BST_UNCHECKED}
  ${EndIf}

  ReadRegStr $RegVal_uninstallString HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallString"
  ReadRegStr $RegVal_uninstallLocation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UnInstallLocation"
  ReadRegStr $RegVal_uninstallVersion HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallVersion"
  ReadRegStr $RegVal_uninstallBitness HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "UninstallBitness"
  ReadRegStr $RegVal_rexxEditor HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxEditor"
  ReadRegStr $RegVal_rexxAssociation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxAssociation"
  ReadRegStr $RegVal_rexxHideAssociation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxHideAssociation"
  ReadRegStr $RegVal_rexxPawsAssociation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxPawsAssociation"
  ReadRegStr $RegVal_sendTo_rexx HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "SendToRexx"
  ReadRegStr $RegVal_sendTo_rexxHide HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "SendToRexxHide"
  ReadRegStr $RegVal_sendTo_rexxPaws HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "SendToRexxPaws"
  ReadRegStr $RegVal_desktop_icon HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "CreateDesktopIcon"

  ; When the uninstaller and current installer are for different addressing
  ; modes, we have had reports of problems.  We try to prevent that by giving
  ; the user the chance to uninstall separately, then rerun the installer.  This
  ; can only happen if the current platform is 64-bit.  And, I think it can only
  ; happen if rxapi is *not* installed as a service.  We check for the former,
  ; but not the later.
  ${If} ${RunningX64}
    Call CheckBitnessMatch
    ${If} $UserRequestAbort == 'true'
      abort
    ${EndIf}
  ${EndIf}

  ; Check for previous version and if the upgrade type of uninstall is available.
  Call CheckInstalledStatus

  ; If a previous version is installed, make sure the key files are not locked.
  ; If they are, we give the user a chance to retry, othwerwise we abort
  ;unconditionally.
  StrCpy $CheckRxApiLock 'false'
  ${If} $PreviousVersionInstalled == 'true'
    Call CheckLockedFiles
    ${If} $UserRequestAbort == 'true'
      abort
    ${EndIf}
  ${EndIf}

  ; Set the send to dialog control variables.
  Call SetSendToVars

  ; Set all the dialog control variables for doing file associations to their
  ; correct starting values.
  Call SetFileAssociationVars

  ; We never do an upgrade type of install unless the user requests it.  But, we
  ; set the default for doing it quick to true.
  StrCpy $DoUpgrade 'false'
  StrCpy $Do_Upgrade_Type_CK_state ${BST_UNCHECKED}
  StrCpy $DoUpgradeQuick 'true'
  StrCpy $Do_Upgrade_Quick_CK_state ${BST_CHECKED}

FunctionEnd

/** Uninstall_Old_ooRexx_page()  Custom page function.
 *
 * This page is show when a previously installed version ooRexx is detected.  It
 * explains to the user that it is necessary to uninstall the previous version
 * and forces the user to check a secondary check box to "force" the install to
 * continue without uninstalling the previous version.
 *
 * The intent is to make it difficult to not do the uninstall and ensure that
 * the user knows installing over the top of an existing install is not
 * supported.
 */
Function Uninstall_Old_ooRexx_page

  /* Skip this page if no previous version is present */
  ${If} $PreviousVersionInstalled == "false"
    Abort
  ${EndIf}

  !insertmacro MUI_HEADER_TEXT "Previous ooRexx Version." "A previous version of ${LONGNAME} is already installed."
  nsDialogs::Create /NOUNLOAD 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateLabel} 0 0 100% 40u \
    "A version of ${LONGNAME} is currently installed.  If the previous version is \
    not uninstalled it is almost certain to cause problems.$\n$\n\
    To uninstall the previous version check the check box. To skip the uninstall step, \
    uncheck the check box."
  Pop $Label_One

  ${NSD_CreateCheckBox} 0 44u 100% 8u "Uninstall previous version of ooRexx"
  Pop $Uninstall_Previous_CK
  ${NSD_Check} $Uninstall_Previous_CK
  ${NSD_OnClick} $Uninstall_Previous_CK ShowHideForceInstall

  ${NSD_CreateLabel} 0 70u 100% 48u \
    "There are very few cases where installing ${LONGNAME} without removing the previous \
    version will work.  If you think you know better than this, proceed with the installation. \
    However, the ${SHORTNAME} developers will not support this type of installation$\n$\n\
    To proceed with the install without uninstalling the previous version of ooRexx you must \
    check the Force install to continue check box."
  Pop $Label_Two
  ShowWindow $Label_Two ${SW_HIDE}

  ${NSD_CreateCheckBox} 0 122u 100% 8u "Force install to continue"
  Pop $Force_Install_CK
  ${NSD_Uncheck} $Force_Install_CK
  ShowWindow $Force_Install_CK ${SW_HIDE}

  nsDialogs::Show

FunctionEnd

/** ShowHideForceInstall()  Callback function.
 *
 * Called when the Unistall previous ooRexx version check box is clicked.
 *
 * If the check box is checked, the user wants to uninstall and we hide the
 * force check box.  If the user unchecks the uninstall check box we show the
 * foce check box.
 */
Function ShowHideForceInstall
	Pop $Uninstall_Previous_CK
	${NSD_GetState} $Uninstall_Previous_CK $0

	${If} $0 == 1
		ShowWindow $Label_Two ${SW_HIDE}
		ShowWindow $Force_Install_CK ${SW_HIDE}
	${Else}
		ShowWindow $Label_Two ${SW_SHOW}
		ShowWindow $Force_Install_CK ${SW_SHOW}
    ${NSD_Uncheck} $Force_Install_CK
	${EndIf}
FunctionEnd

/** OpenUninstallLog()
 *
 * Opens up the uninstall log.  In the hidden OpenLogFile section, the $INSTDIR
 * must be created before this function is callled.
 */
Function OpenUninstallLog
  IfFileExists "$INSTDIR\${UninstLog}" +3
    FileOpen $UninstLog "$INSTDIR\${UninstLog}" w
    Goto +4
  SetFileAttributes "$INSTDIR\${UninstLog}" NORMAL
  FileOpen $UninstLog "$INSTDIR\${UninstLog}" a
  FileSeek $UninstLog 0 END
FunctionEnd

/** CloseUninstallLog()
 *
 * Closes up the uninstall log.
 */
Function CloseUninstallLog
  FileClose $UninstLog
  SetFileAttributes "$INSTDIR\${UninstLog}" READONLY|SYSTEM|HIDDEN
FunctionEnd

/** Uninstall_Old_ooRexx_Leave()  Callback function.
 *
 * The installer calls this function when the user presses the Next button on
 * the uninstall old ooRexx version page.
 */
Function Uninstall_Old_ooRexx_Leave
	${NSD_GetState} $Uninstall_Previous_CK $0
	${NSD_GetState} $Force_Install_CK $1

  /* If the user said to not uninstall the previous and did not check force the
   * install, then send them back to the page.
   */
  ${If} $0 == 0
  ${andif} $1 == 0
    MessageBox MB_OK \
      "To bypass uninstalling the previous version of ooRexx you must check$\n\
      the Force install check box.  If it is not the intention to skip the$\n\
      uninstall step, then the Uninstall previous version must be checked.$\n$\n\
      Please check the appropriate check boxes to continue." \
      /SD IDOK
      Abort
  ${EndIf}

  /* If the user said to not uninstall the previous and to force the install,
   * then we skip the uninstall.  To signal this we set the
   * PreviousVersionInstalled variable to false.
   */
  ${If} $0 == 0
  ${andif} $1 == 1
    StrCpy $PreviousVersionInstalled 'false'
  ${EndIf}

FunctionEnd

/** Uninstall_Type_page()  Custom page function.
 *
 * This page is show when a previously installed version ooRexx is detected,
 * and the uninstaller version is capable of doing an upgrade type of install.
 *
 * It gives the user the option of only deleting the installed files while
 * leaving the environment, registry, etc., setting untouched.
 */
Function Uninstall_Type_page

  /* Skip this page if no previous version is present */
  ${If} $PreviousVersionInstalled == "false"
    Abort
  ${EndIf}

  ${If} $UpgradeTypeAvailable == 'false'
    Call Do_The_Uninstall
    Abort
  ${EndIf}

  !insertmacro MUI_HEADER_TEXT "Upgrade Previous ooRexx Version." "It is possible to do an 'upgrade' type of install."
  nsDialogs::Create /NOUNLOAD 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}


  ${NSD_CreateLabel} 0u 0u 100% 64u \
    "An 'upgrade' type of install will remove all the previously installed files and install \
    the new version of ${SHORTNAME}.  However, it will not undo any of the environment, file \
    association, or registry settings done by the previous install.$\n$\n\
    Use this type of install to quickly replace the old version files with the new version \
    files. Or, when you have customized these types of settings and do not wish them changed.$\n$\n\
    Note: with this type of install, the install location and other settings can NOT be changed.$\n$\n\"
  Pop $Label_One

  ${NSD_CreateCheckBox} 0u 68u 100% 8u "Perform an upgrade type of install"
  Pop $Do_Upgrade_Type_CK

  ${NSD_CreateLabel} 0u 92u 100% 24u \
    "A quick upgrade type of install will skip the pages pertaining to options that can not be changed.  \
    Otherwise the option pages are shown so the value of the options can be seen, but it is still not \
    possible to change the values."
  Pop $Label_Two

  ${NSD_CreateCheckBox} 0u 120u 100% 8u "Perform a quick upgrade type of install"
  Pop $Do_Upgrade_Quick_CK

  ${NSD_OnBack} Uninstall_Type_onBack
  ${NSD_OnClick} $Do_Upgrade_Type_CK ShowHideQuickUninstall

	${NSD_SetState} $Do_Upgrade_Type_CK $Do_Upgrade_Type_CK_state
	${NSD_SetState} $Do_Upgrade_Quick_CK $Do_Upgrade_Quick_CK_state

  ${If} $Do_Upgrade_Type_CK_state == ${BST_UNCHECKED}
		ShowWindow $Label_Two ${SW_HIDE}
		ShowWindow $Do_Upgrade_Quick_CK ${SW_HIDE}
  ${EndIf}

  ; Set focus to the page dialog rather than the installer dialog, set focus to
  ; the check box, and then show the page dialog
  SendMessage $Dialog ${WM_SETFOCUS} $HWNDPARENT 0
  SendMessage $Dialog ${WM_NEXTDLGCTL} $Do_Upgrade_Type_CK 1

  nsDialogs::Show

FunctionEnd

/** Uninstall_Type_onBack()  Call back function
 *
 * Invoked when the user clicks the back button on the Uninstall type page.  We
 * save the state of the check boxes on the page.
 */
Function Uninstall_Type_onBack
	${NSD_GetState} $Do_Upgrade_Type_CK $Do_Upgrade_Type_CK_state
	${NSD_GetState} $Do_Upgrade_Quick_CK $Do_Upgrade_Quick_CK_state
FunctionEnd

/** Uninstall_Type_Leave()  Call back function.
 *
 * The installer calls this function when the user presses the Next button on
 * the uninstall type page.  We don't save the check box state because we
 * immediately run the uninstall and the user won't see this page again.
 * Rather, we set the DoUpgrade / DoUpgradeQuick variables to the proper values.
 */
Function Uninstall_Type_Leave
	${NSD_GetState} $Do_Upgrade_Type_CK $Do_Upgrade_Type_CK_state
	${NSD_GetState} $Do_Upgrade_Quick_CK $Do_Upgrade_Quick_CK_state

  ${If} $Do_Upgrade_Type_CK_state == ${BST_CHECKED}
    StrCpy $DoUpgrade 'true'

    ${If} $Do_Upgrade_Quick_CK_state = ${BST_CHECKED}
      StrCpy $DoUpgradeQuick 'true'
    ${else}
      StrCpy $DoUpgradeQuick 'false'
    ${EndIf}
  ${else}
    StrCpy $DoUpgrade 'false'
    StrCpy $DoUpgradeQuick 'false'
  ${EndIf}

  Call Do_The_Uninstall

FunctionEnd

/** ShowHideQuickUninstall() Call back function
 *
 * Invoked when the user clicks the Perform an upgrade type of install check
 * box.  Shows or hides the Perform a quick upgrade type of install check box.
 */
Function ShowHideQuickUninstall
	Pop $Do_Upgrade_Type_CK
	${NSD_GetState} $Do_Upgrade_Type_CK $0

	${If} $0 == ${BST_CHECKED}
		ShowWindow $Label_Two ${SW_SHOW}
		ShowWindow $Do_Upgrade_Quick_CK ${SW_SHOW}
	${else}
		ShowWindow $Label_Two ${SW_HIDE}
		ShowWindow $Do_Upgrade_Quick_CK ${SW_HIDE}
	${EndIf}
FunctionEnd

/** Do_The_Uninstall()
 *
 * Executes the uninstall program and waits for its return.  The user could
 * have:
 *    Finished the uninstall
 *    Canceled the uninstall because they want to stop any running Rexx programs
 *    Canceled the uninstll for some other reason
 *
 * In addition, the script may have aborted the uninstall for some unexplained
 * error.
 *
 * In all cases we quit the install.  We put up a message box to explain why we
 * are quitting in most cases, the only exception is if the user choose to quit
 * to stop any running Rexx programs.
 *
 * @note  If we are doing an upgrade install, then we need to install to the
 *        same location as the original install.  This could easily be different
 *        than what $INSTDIR currently is, so we set $INSTDIR to our uninstall
 *        location.  For an upgrade install, the portion of the installer that
 *        allows the user to pick the install location will be disabled.
 */
Function Do_The_Uninstall

  ; Use $3 for the arg to the uninstall program.
  ${If} $DoUpgrade == 'true'
    StrCpy $INSTDIR $RegVal_uninstallLocation
    StrCpy $3 "upgrade"

    ${If} $DoUpgradeQuick == 'true'
      StrCpy $3 "upgradeQuick"
    ${EndIf}
  ${EndIf}

  HideWindow
  ClearErrors

  /* the "_?=$RegVal_uninstallLocation" portion of the command sets the
   * uninstall dir to $R2 and *prevents* the uninstaller from running in the
   * temp dir, which is what we want.
   */
  ExecWait '$RegVal_uninstallString /U=$3 _?=$RegVal_uninstallLocation' $0

  IfErrors UninstallErrors
  ${If} $0 == 0
    ; No errors, do a sanity check and finish up.
    IfFileExists "$INSTDIR\${KEYFILE1}" UninstallErrors
    IfFileExists "$INSTDIR\${KEYFILE2}" UninstallErrors
    Delete "$RegVal_uninstallLocation\${UNINSTALLER}"
    RMDir "$RegVal_uninstallLocation"

    BringToFront
    Goto NotInstalled
  ${EndIf}

  ${If} $0 == 1
    ; The user canceled the uninstall ...
    MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST \
               "You have elected to cancel the uninstall of the previous version of$\n\
               ${SHORTNAME}.  There are very few cases where installing ${LONGNAME}$\n\
               without removing the previous version will work and this installation$\n\
               will abort.$\n$\n\
               If you are determined to install ${SHORTNAME} without removing the$\n\
               previous version, rerun the installation and select to not uninstall$\n\
               the previous version.  Then select to force the installation." \
               /SD IDOK
    Goto DoAbort
  ${EndIf}

  ${If} $0 == 4
    ; The user wants to quit to stop rxapi.  Only set to 4 if the user cancels
    ; on the page where she is asked to stop rxapi.  On that page she is given
    ; the option to quit completely.
    Goto DoAbort
  ${EndIf}

  ${If} $0 == 5
    ; 5 is only set when there are locked files
    MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST \
               "The uninstall of the previous version of ${SHORTNAME} was terminated by$\n\
               the uninstall script.  The most likely cause of this is that there are$\n\
               Rexx programs still running, which prevents critical files from being$\n\
               updated.$\n$\n\
               The installation will abort.$\n$\n\
               You must ensure all Rexx programs are ended.  Then restart the installation." \
               /SD IDOK

    Goto DoAbort
  ${EndIf}

  ; Return code is 2 or greater, but not 4 or 5.  2 is uninstall canceled by
  ; script, we treat anything greater than 2 in the same way.
  MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST \
             "The uninstall of the previous version of ${SHORTNAME} was terminated by$\n\
             the uninstall script.  The most likely cause of this is that the rxapi$\n\
             process is running, either as a service or stand alone, and could not be$\n\
             stopped.$\n$\n\
             When the previous version of rxapi is not stopped, the new version of$\n\
             of ${SHORTNAME} can not be installed correctly.$\n$\n\
             The installation will abort.$\n$\n\
             You can:$\n$\n\
             1.) Contact the developers on the SourceForge project for help.$\n$\n\
             2.) Try stopping rxapi manually and rerunning the installation.  If$\n\
             rxapi is installed as a service, use the service manager to stop rxapi.$\n\
             Otherwise, use the task manager to stop the rxapi process.$\n$\n\
             3.) Elect to not stop rxapi and to not remove the previous version.  To$\n\
             do this, rerun the installation and click No when asked to uninstall$\n\
             the previous version.  Then click Yes when asked if you want to$\n\
             continue.  However, if you do this ${SHORTNAME} will not be installed$\n\
             correctly." \
             /SD IDOK
  Goto DoAbort

  UninstallErrors:
    MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST \
               "There were unexpected errors uninstalling the previous version$\n\
               of ${SHORTNAME}.  There are very few cases where installing ${LONGNAME}$\n\
               without removing the previous version will work and this installation$\n\
               will abort.  You should first try rerunning the installtion.$\n$\n\
               If this is the second attempt at installation and you still get this$\n\
               message, you can:$\n$\n\
               1.) Contact the developers on the SourceForge project for help.$\n$\n\
               2.) Try removing the previous version manually and rerun the installation.$\n$\n\
               3.) Elect to not remove the previous version.  To do this, rerun the installation$\n\
               and click No when asked to uninstall the previous version.  Then click Yes when$\n\
               asked if you want to continue.$\n$\n\
               In all cases, please report this problem to the ${SHORTNAME} project on SourceForge." \
               /SD IDOK
    Goto DoAbort

  DoAbort:
    Quit

  NotInstalled:
  ; The previous version is now uninstalled, so we set the variable to reflect that.
  StrCpy $PreviousVersionInstalled 'false'
FunctionEnd

/** Ok_Stop_RxAPI_page()  Custom page function.
 *
 * This is a custom page that is shown if rxapi is still running.  If rxapi.exe
 * is not running the page is skipped.  On the page, the user is asked for the
 * okay to stop rxapi and advised of possible consequences.  If the user says
 * no, the install will be aborted..
 */
Function Ok_Stop_RxAPI_page
  Call CheckIsRxapiService
  Call CheckIsRxapiRunning

  ${If} $RxapiIsRunning == 'false'
    Abort
  ${EndIf}

  !insertmacro MUI_HEADER_TEXT "The ${LONGNAME} memory manager (rxapi) is currently running." \
                               "A new version of rxapi can not be installed while rxapi is running."
  nsDialogs::Create /NOUNLOAD 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateLabel} 0 0 100% 112u \
    "A previous version of the ${LONGNAME} memory manager (rxapi) is currently running. \
    The new version of rxapi can not be installed while the previous version is running.$\n$\n\
    When the Stop rxapi check box is checked, the installation will stop rxapi before \
    proceeding.  If the check box is not checked, rxapi will not be stopped.$\n$\n\
    If rxapi is not stopped the install will be canceled.$\n$\n\
    If, and only if, there are Rexx programs running, stopping the memory manager could \
    possibly cause data loss.$\n$\n\
    If you are worried about this, please uncheck the Stop rxapi check box.  When the \
    install ends, stop all running Rexx programs, and rerun the installation program."

  Pop $Label_One

  ${NSD_CreateCheckBox} 0 116u 100% 15u "Stop rxapi"
  Pop $StopRxAPI_CK
  ${NSD_SetState} $StopRxAPI_CK 1

  nsDialogs::Show

FunctionEnd

/** Ok_Stop_RxAPI_leave()  Call back function.
 *
 * Invoked when the user leaves the page that asks for permission to stop the
 * rxapi process.  We check if the user said okay or not.  If the user does not
 * say okay we quit the install.
 *
 * When the user does say okay, we stop rxapi and check that it is indeed
 * stopped.  If it is not stopped, we inform the user and also quit the install.
 */
Function Ok_Stop_RxAPI_leave

  ${NSD_GetState} $StopRxAPI_CK $StopRxAPI_CK_State

  ${If} $StopRxAPI_CK_State == 0
    Quit
  ${EndIf}

  Call StopRxapi
  Pop $R0
  ${If} $R0 == 'Ok'
    Goto NotRunning
  ${EndIf}

  ; rxapi was not stopped, the error code is now at top of stack
  Pop $R0

  MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST \
             "Can not determine conclusively that the Open Object Rexx memory manager (rxapi) is$\n\
             stopped.  Since a new version of rxapi can not be installed while rxapi is running,$\n\
             the installation will quit.$\n$\n\
             Please use the task manager to locate rxapi.exe and end that process, then restart the$\n\
             installation.  If you are uncomfortable with using the task manager, contact the$\n\
             ${SHORTNAME} developers on SourceForge for help.$\n$\n\
             If you report this problem, please note that the error code is $R0." /SD IDOK
    Quit

  NotRunning:

FunctionEnd

/** Components_Page_pre()  Call back function
 *
 * Invoked by the installer before the components page is created.  We do one
 * last check that there are no locked files.  If there are we quit.  This could
 * be frustrating for a user, but we know that things will fail if we proceed.
 *
 * If this is a QUICK upgrade install, we skip the page.
 */
Function Components_Page_pre

  StrCpy $CheckRxApiLock 'false'
  Call CheckLockedFiles
  ${If} $$UserRequestAbort == 'true'
    Quit
  ${EndIf}

  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}

FunctionEnd

/** Components_Page_show()  Call back function
 *
 * Invoked by the installer right before the components page is shown.
 *
 * If this is an upgrade install, we don't allow the user to quit from here on
 * out.  On this page we also don't let the user go back
 *
 * When it is not an upgrade install, nothing is done.
 */
Function Components_Page_show

  ${If} $DoUpgrade == 'true'
    ; Disable Back button
    GetDlgItem $0 $HWNDPARENT 3
    EnableWindow $0 0

    ; Disable the components list view control
    EnableWindow $mui.ComponentsPage.Components 0

    ; Change the descriptive text
    SendMessage $mui.ComponentsPage.Text ${WM_SETTEXT} 0 \
      "STR:These components of ${LONGNAME} ${VERSION} wil be installed.  The component list can not be \
      changed for an upgrade type of install.  Click Next to continue."

    Call PageDisableQuit
  ${EndIf}

FunctionEnd

/** Directory_Page_pre()  Call back function
 *
 * Invoked by the installer before the directory page is created.
 *
 * If this is a QUICK upgrade install, we skip the page.
 */
Function Directory_Page_pre

  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}

FunctionEnd

/** Directory_Page_show()  Call back function
 *
 * Invoked by the installer right before the directory page is shown.
 *
 * If this is an upgrade install, the install location can not be changed.  We
 * use function to show the user the install location, but not let them change
 * the location.
 *
 * When it is not an upgrade install, nothing is done.
 */
Function Directory_Page_show

  ${If} $DoUpgrade == 'true'
    SendMessage $mui.DirectoryPage.Text ${WM_SETTEXT} 0 \
      "STR:Setup will install ${LONGNAME} ${VERSION} in the following folder.  The location can not be \
      changed for an upgrade type of install.  Click Next to continue."

    EnableWindow $mui.DirectoryPage.Directory 0
    EnableWindow $mui.DirectoryPage.BrowseButton 0

    Call PageDisableQuit
  ${EndIf}

FunctionEnd

/** Finish_Page_pre()  Call back function
 *
 * Invoked by the installer before the finish page is created.
 *
 * If this is an upgrade install, the creation or not, of the desktop icon can
 * not be changed.  So we always create the icon here if we are going to.
 *
 * If this is a QUICK upgrade install, we abort here so that the finish page is
 * not shown at all
 *
 * At the time this is run, the registry values have been deleted.  The value in
 * RegVal_desktop_icon is the value in the registry when this installer was
 * started.  If there is an existing ooRexx, the ooRexx uninstaller is run and
 * it always deletes all the existing registry entries.  If we are doing an
 * upgrade and creating the icon we write a 1 for the registry value.  If we are
 * not creating the icon we write a 0 just to be tidy.  Leaving out the entry
 * altogether would be fine.  If we are not doing an upgrade, we do not do
 * anything here, the Finish page will be shown and the CreateDesktopIcon()
 * function will do the appropriate thing.
 */
Function Finish_Page_pre

  ${If} $DoUpgrade == 'true'
    ${If} $RegVal_desktop_icon == '1'
      CreateShortcut "$DESKTOP\Open Object Rexx Resources.lnk" "$SMPROGRAMS\${LONGNAME}\" "" "$INSTDIR\rexx.exe"
      Call OpenUninstallLog
      ${AddItem} "$DESKTOP\Open Object Rexx Resources.lnk"
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "CreateDesktopIcon" 1
      Call CloseUninstallLog
    ${else}
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "CreateDesktopIcon" 0
    ${EndIf}
    ${If} $DoUpgradeQuick == 'true'
      Abort
    ${EndIf}
  ${EndIf}

FunctionEnd

/** Finish_Page_show()  Call back function
 *
 * Invoked by the installer right before the finish page is shown.
 *
 * Note that if this is a quick upgrade type, we never get here.
 *
 * If this is an upgrade install, the create desktop icon value can not be
 * changed.  We use function to show the create desktop icon value by checking
 * or not checking the check box, but we disable the check box.  For the show
 * getting started with Windows ooRex checkbox, we assume on an upgrade install,
 * the user doesn't need this at all and we make the check box invisible.
 *
 * When it is not an upgrade install, nothing is done.
 */
Function Finish_Page_show

  ${If} $DoUpgrade == 'true'
    SendMessage $mui.FinishPage.Text ${WM_SETTEXT} 0 \
      "STR:${LONGNAME} ${VERSION} has been installed on your computer.  Whether a Desktop \
      Icon has been created or not is indicated by the state of the check box. This can not \
      be changed for an upgrade type of install.$\n$\n  Click Finish to close this Wizard."

    SendMessage $mui.FinishPage.Run ${BM_SETCHECK} ${BST_CHECKED} $RegVal_desktop_icon
    EnableWindow $mui.FinishPage.Run 0

    ShowWindow $mui.FinishPage.ShowReadme ${SW_HIDE}

    ; Show or not show the link?
    ShowWindow $mui.FinishPage.Link ${SW_HIDE}

  ${EndIf}

FunctionEnd

/** Rxapi_Options_page()  Custom page function.
 *
 * This function is used to display the custom page that asks the user if they
 * want to install rxapi as a service, and if they want rxapi started when the
 * installation finishes.  It checks if the user is an admin and skips the
 * page if not.  (Admin privileges are required to install a service. ? is that
 * true?)
 */
Function Rxapi_Options_page

  ${If} $IsAdminUser == 'false'
    Abort
  ${EndIf}

  ; If doing an upgrade and rxapi was NOT previously installed as a service, we
  ; skip this page.  If we are doing a quick upgrade and rxapi was installed as
  ; a server, we start it automatically.
  ${If} $DoUpgrade == 'true'
    ${If} $RxapiIsService == 'false'
      Abort
    ${EndIf}
    ${If} $DoUpgradeQuick == 'true'
    ${andif} $RxapiIsService == 'true'
      StrCpy $RxAPIStartService '1'
      Abort
    ${EndIf}
  ${EndIf}

  ${If} $DoUpgrade == 'true'
    !insertmacro MUI_HEADER_TEXT "ooRexx rxapi is installed as a Windows Service." \
                                 "Installation of rxapi as a service can not be changed during an upgrade, but \
                                 you can choose whether to start the rxapi Service during installation."
  ${else}
    !insertmacro MUI_HEADER_TEXT "The ooRexx rxapi process." "Install rxapi as a Windows Service."
  ${EndIf}

  nsDialogs::Create /NOUNLOAD 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_OnBack} Rxapi_Options_leave

  ${NSD_CreateLabel} 0 0 100% 80u \
    "ooRexx starts a daemon process, rxapi.exe, the first time a Rexx program executes.  This \
    process manages all data that can persist across interpreter invocations or is used for \
    cross-process communications.  The rxapi process manages the Rexx data queues, the macrospace, \
    and all of the external function, subcommand handler and exit registrations.$\n$\n\
    On Windows, in all cases, it is best to install rxapi as a Windows service.  In Windows Vista \
    and later operating systems, it is strongly recommmended that rxapi is always installed as a \
    service."
  Pop $Label_One

  ${NSD_CreateCheckBox} 0 86u 100% 15u "Install rxapi as a Windows Service"
  Pop $RxAPI_Install_Service_CK

  ; If we are doing an upgrade, then we are only here if rxapi was previously
  ; installed as a service.  If so we don't allow the user to change this.
  ${If} $DoUpgrade == 'true'
    ${NSD_SetState} $RxAPI_Install_Service_CK 1
	  EnableWindow $RxAPI_Install_Service_CK 0
	  EnableWindow $Label_One 0
    Call PageDisableQuit
  ${else}
    ${NSD_SetState} $RxAPI_Install_Service_CK $RxAPIInstallService
    ${NSD_OnClick} $RxAPI_Install_Service_CK EnableStartService
  ${EndIf}

  ${NSD_CreateCheckBox} 0 106u 100% 15u "Start the rxapi Service during installation"
  Pop $RxAPI_Start_CK
  ${NSD_SetState} $RxAPI_Start_CK $RxAPIStartService
  ${If} $RxAPIInstallService == 0
	  EnableWindow $RxAPI_Start_CK 0
  ${EndIf}

  nsDialogs::Show
FunctionEnd

/** EnableStartService()  Callback function.
 *
 * Called when the install rxap as a service check box is clicked.
 *
 * If the check box is checked, we enable the start the service check box, if it
 * is unchecked we disable the check box.
 *
 * Note: If we are doing an upgrade, then NSD_OnClick is not set.  So we don't
 *       need to worry about getting a click during an upgrade install.
 */
Function EnableStartService
	Pop $RxAPI_Install_Service_CK
	${NSD_GetState} $RxAPI_Install_Service_CK $0

	${If} $0 == 1
	  EnableWindow $RxAPI_Start_CK 1
    ${NSD_SetState} $RxAPI_Start_CK $RxAPIStartService
	${Else}
	  EnableWindow $RxAPI_Start_CK 0
    ${NSD_Uncheck} $RxAPI_Start_CK
	${EndIf}
FunctionEnd

/** Rxapi_Options_leave()  Call back function.
 *
 * Called by the installer when the user clicks Next or Back buttons on the
 * Rxapi_Options_page page.  We set the RxAPI option variables to match what the
 * user picked.
 */
Function Rxapi_Options_leave
  ${NSD_GetState} $RxAPI_Install_Service_CK $RxAPIInstallService
  ${NSD_GetState} $RxAPI_Start_CK $RxAPIStartService
FunctionEnd

/** File_Associations_page()  Custom page function.
 *
 * Allows the user to specify whether they want to use Windows file associations
 * or not.
 *
 * The page is not shown when doing an upgrade type of install.
 */
Function File_Associations_page

  ${If} $DoUpgrade == 'true'
    Abort
  ${EndIf}

  !insertmacro MUI_HEADER_TEXT "Associate file extensions with the ooRexx executables." \
                               "Any, or all, of the ooRexx executables (rexx.exe, rexxhide.exe, or rexxpaws.exe) \
                               can be associated with a file extension."

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_OnBack} File_Associations_leave

  ${NSD_CreateLabel} 0 0 100% 120u \
    "Windows file associations define how Windows treats a file type on your system. \
    File associations control, among other things, what happens when a user double-clicks a \
    file, which icon appears for a file by default, and how the file appears when viewed \
    in Windows Explorer.$\n$\n\
    Associating a file extension with an ooRexx executable allows Rexx programs to be executed by \
    double clicking them in Windows explorer, and to be run from the command line by just typing \
    their file name.  In addition, you can specify an editor for the file type.  This editor is \
    used when the Edit item of the context menu for the file is selected.$\n$\n\
    Check the check box to have the installer create file associations.  The following pages \
    will allow you to specify which associations are created.  Uncheck the check box if you \
    do not want any associations to be created."

  Pop $Label_One

  ${NSD_CreateCheckBox} 0 122u 100% 8u "Create Windows file associations"
  Pop $Use_File_Associations_CK
  ${NSD_SetState} $Use_File_Associations_CK $Use_File_Associations_CK_state

  nsDialogs::Show

FunctionEnd

/** File_Associations_leave()  Call back function.
 *
 * Invoked when the user clicks the Back or Next buttons on the File
 * Associations page.  Saves the state of the Use File Associations check box.
 */
Function File_Associations_leave

  ${NSD_GetState} $Use_File_Associations_CK $Use_File_Associations_CK_state

FunctionEnd

/** SendTo_Items_page()  Custom page function.
 *
 * Allows the user to create 'Send To' items for the Rexx executables.
 *
 * The page is skipped if we are doing a quick upgrade.
 */
Function SendTo_Items_page

  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
    !insertmacro MUI_HEADER_TEXT "The Send To items that will be created." \
                                 "Which Send To items will be created can not be changed during an upgrade \
                                 type of install."
  ${else}
    !insertmacro MUI_HEADER_TEXT "Create 'Send To' items for the Rexx executables." \
                                 "'Send To' items can be used instead of file associations, or in addition \
                                 to file associations."
  ${EndIf}

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_OnBack} SendTo_Items_leave

  ${NSD_CreateLabel} 0u 0u 100% 72u \
    "The 'Send To' context menu item for objects displayed in the Windows Explore contains a list of \
    programs that the object can be 'sent to.'  Typically, the items in the list are executables that \
    can process the file or directory object sent to them.  'Sending' your Rexx program to one of the \
    Rexx executables will execute your program.$\n$\n\
    Rather than create a file association for rexxhide.exe and rexxpaws.exe you may wish to create a \
    'Send To' item for those executables.  Or create both a file association and 'Send To' items for \
    those executables.  Usually a file association is created for rexx.exe and not a 'Send To' item."

  Pop $Label_One

  ${NSD_CreateCheckBox}   0u   80u 100%  8u "Create 'Send To' Rexx item  (Typically this is not done.)"
  Pop $SendTo_rexx_CK

  ${NSD_CreateCheckBox}    0u 100u 100%  8u "Create 'Send To' Rexx Hide (rexxhide.exe) item"
  Pop $SendTo_rexxHide_CK

  ${NSD_CreateCheckBox}    0u 120u 100%  8u "Create 'Send To' Rexx Pause (rexxpaws.exe) item"
  Pop $SendTo_rexxPaws_CK

  ${NSD_SetState} $SendTo_rexx_CK $SendTo_rexx_CK_state
  ${NSD_SetState} $SendTo_rexxhide_CK $SendTo_rexxHide_CK_state
  ${NSD_SetState} $SendTo_rexxpaws_CK $SendTo_rexxPaws_CK_state

  ${If} $DoUpgrade == 'true'
    EnableWindow $Label_One 0
	  EnableWindow $SendTo_rexx_CK 0
	  EnableWindow $SendTo_rexxHide_CK 0
	  EnableWindow $SendTo_rexxPaws_CK 0
    Call PageDisableQuit
	${EndIf}

  nsDialogs::Show

FunctionEnd

/** SendTo_Items_leave()  Call back function.
 *
 * This function is called when the Send To Items page is left.  The state of
 * the dialog controls is saved.
 */
Function SendTo_Items_leave

  ${NSD_GetState} $SendTo_rexx_CK $SendTo_rexx_CK_state
  ${NSD_GetState} $SendTo_rexxHide_CK $SendTo_rexxHide_CK_state
  ${NSD_GetState} $SendTo_rexxPaws_CK $SendTo_rexxPaws_CK_state

FunctionEnd

/** Associate_rexx_page()  Custom page function.
 *
 * Presents the option to the user of creating a file association for rexx.exe
 *
 * The page is skipped if not using file associations, or if we are doing a
 * quick upgrade.
 */
Function Associate_rexx_page

  ${If} $Use_File_Associations_CK_state == ${BST_UNCHECKED}
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
    !insertmacro MUI_HEADER_TEXT "The file extension for rexx.exe and Rexx program editor choices." \
                                 "These choices can not be changed during an upgrade type of install."
  ${else}
    !insertmacro MUI_HEADER_TEXT "Associate a file extension with rexx.exe." "Pick the editor to be used when the Edit \
                                 item of the context menu is selected."
  ${EndIf}

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_OnBack} Associate_rexx_leave

  ${NSD_CreateLabel} 0u 0u 100% 16u \
    "Select the editor to be used with your ooRexx file type(s).  This adds the Edit menu item to \
    the context menu of the file type.  The same editor is used for all file types."

  Pop $Label_One

  ; Editor line
  ${NSD_CreateLabel}     0u 20u 60u  8u "Full path to editor:"
  Pop $0  ; Discarded

  ${NSD_CreateText}      0u 30u 185u 12u $Rexx_editor_text
  Pop $Rexx_editor_EDIT

  ${NSD_CreateButton}   190u 28u  45u 16u "Browse..."
  Pop $Rexx_editor_PB

	System::Call shlwapi::SHAutoComplete(i$Rexx_editor_EDIT, i${SHACF_FILESYSTEM})


  ${NSD_CreateLabel} 0u 60u 100% 24u \
    "By default in ${LONGNAME} installations the Rexx interpreter, rexx.exe, is associated with the \
    ${DefRexxExt} extension using the file type name of ${DefRexxFType}.  However, you can change these values if \
    you care to."

  Pop $Label_Two

  ${NSD_CreateGroupBox} 0u 90u 100% 44u "File Association for rexx.exe"
  Pop $0  ; Discarded

  ${NSD_CreateCheckBox} 8u  104u 80% 8u "Create rexx.exe file association"
  Pop $Associate_rexx_CK

  ${NSD_SetState} $Associate_rexx_CK $Associate_rexx_CK_state

  ; Extension line
  ${NSD_CreateLabel}     16u  116u  75u  8u "Extension (no spaces):"
  Pop $0  ; Discarded

  ${NSD_CreateText}      91u  114u  36u 12u $Rexx_ext_text
  Pop $Rexx_ext_EDIT

  ; File type line
  ${NSD_CreateLabel}     137u 116u  90u  8u "File type name (no spaces):"
  Pop $0  ; Discarded

  ${NSD_CreateText}      228u 114u  46u 12u $Rexx_ftype_text
  Pop $Rexx_ftype_EDIT

  ${NSD_SetState} $Associate_rexx_CK $Associate_rexx_CK_state
  ${NSD_SetState} $SendTo_rexx_CK $SendTo_rexx_CK_state

  ${If} $Associate_rexx_CK_state == ${BST_UNCHECKED}
  ${orif} $DoUpgrade == 'true'
	  EnableWindow $Rexx_ext_EDIT 0
	  EnableWindow $Rexx_ftype_EDIT 0

    ${If} $DoUpgrade == 'true'
  	  EnableWindow $Rexx_editor_EDIT 0
  	  EnableWindow $Rexx_editor_PB 0
  	  EnableWindow $Associate_rexx_CK 0
      EnableWindow $Label_One 0
      EnableWindow $Label_Two 0
      Call PageDisableQuit
  	${EndIf}
	${EndIf}

  ${NSD_OnClick} $Associate_rexx_CK EnableRexxAssociation
  ${NSD_OnClick} $Rexx_editor_PB Get_rexx_editor_file

  nsDialogs::Show

FunctionEnd

/** Associate_rexx_leave()  Call back function.
 *
 * This function is called when the Rexx association page is left.  We check
 * that the text fields are 'sane' and return the user to the page if they
 * are not.  If the fields are good, the state of the dialog controls is saved.
 */
Function Associate_rexx_leave

  ${NSD_GetState} $Associate_rexx_CK $Associate_rexx_CK_state

  ${If} $Associate_rexx_CK_state == ${BST_CHECKED}
    ${NSD_GetText} $Rexx_ext_EDIT $0
    ${NSD_GetText} $Rexx_ftype_EDIT $1

    ; If either the extension field or the ftype field are blank, abort.
    ${If} $0 == ""
    ${orif} $1 == ""
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Creating a file association requires that$\n\
        both the file extension and the file type field$\n\
        be filed in."

      ${If} $0 == ""
        SendMessage $Dialog ${WM_NEXTDLGCTL} $Rexx_ext_EDIT 1
      ${else}
        SendMessage $Dialog ${WM_NEXTDLGCTL} $Rexx_ftype_EDIT 1
      ${EndIf}
      Abort
    ${EndIf}

    ; Copy the first char of the ext field to $0 if not a '.', abort.
    StrCpy $2 $0 1
    ${If} $2 != "."
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Please include the leading dot '.' of the file$\n\
        extension."
      SendMessage $Dialog ${WM_NEXTDLGCTL} $Rexx_ext_EDIT 1
      Abort
    ${EndIf}

    ; Check that the user did not include any spaces in either field. If so abort
    push $0
    call CheckForSpaces
    pop $2
    push $1
    call CheckForSpaces
    pop $3

    ${If} $2 > 0
    ${orif} $3 > 0
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Neither the file extension field nor the file$\n\
        type name field can contain a space."

      ${If} $2 > 0
        SendMessage $Dialog ${WM_NEXTDLGCTL} $Rexx_ext_EDIT 1
      ${else}
        SendMessage $Dialog ${WM_NEXTDLGCTL} $Rexx_ftype_EDIT 1
      ${EndIf}
      Abort
    ${EndIf}

  ${EndIf}


  ; Okay text fields are okay.  If editor field is blank, it is just not used.
  ${NSD_GetText} $Rexx_ext_EDIT $Rexx_ext_text
  ${NSD_GetText} $Rexx_ftype_EDIT $Rexx_ftype_text
  ${NSD_GetText} $Rexx_editor_EDIT $Rexx_editor_text

FunctionEnd

/** Get_rexx_editor_file()
 *
 * Puts up a file open dialog and allows the user to select a file.  If the
 * user does not cancel, the edit control is updated with the file selected.
 */
Function Get_rexx_editor_file
	Pop $Rexx_editor_PB
	${NSD_GetText} $Rexx_editor_EDIT $0

  nsDialogs::SelectFileDialog open $0 "*.exe"
  pop $0
  ${If} $0 != ""
  	${NSD_SetText} $Rexx_editor_EDIT $0
  ${EndIf}

FunctionEnd

/** EnableRexxAssociation()
 *
 * Called when the user clicks on the Create rexx.exe file association check
 * box.  Disables, or enables, the controls depending on if the check box is
 * checked or not.
 */
Function EnableRexxAssociation
	Pop $Associate_rexx_CK
	${NSD_GetState} $Associate_rexx_CK $0

	${If} $0 == 1
	  EnableWindow $Rexx_ext_EDIT 1
	  EnableWindow $Rexx_ftype_EDIT 1
	${Else}
	  EnableWindow $Rexx_ext_EDIT 0
	  EnableWindow $Rexx_ftype_EDIT 0
	${EndIf}
FunctionEnd

/** Associate_otherExes_page()  Custom page function.
 *
 * This is a custom page
 *
 *
 *
 */
Function Associate_otherExes_page

  ${If} $Use_File_Associations_CK_state == ${BST_UNCHECKED}
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
    !insertmacro MUI_HEADER_TEXT "The file extension chocies for rexxhide.exe. and rexxpaws.exe" \
                                 "These choices can not be changed during an upgrade type of install."
  ${else}
    !insertmacro MUI_HEADER_TEXT "Associate a file extension with rexxhide.exe and / or rexxpaws.exe" \
                                 "rexxhide default file extension: $\"${DefRexxHideExt}$\" file type: $\"${DefRexxHideFType}$\"$\n\
                                 rexpaws default file extension: $\"${DefRexxPawsExt}$\" file type: $\"${DefRexxPawsFType}$\"."
  ${EndIf}

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_OnBack} Associate_otherExes_leave

  /* Controls for Rexx Hide */
  ${NSD_CreateLabel} 0u 0u 100% 8u \
    "rexxhide runs Rexx programs without creating a console window."

  Pop $Label_One

  ${NSD_CreateGroupBox} 0u 12u 100% 44u "File Association for rexxhide.exe"
  Pop $0  ; Discarded

  ${NSD_CreateCheckBox} 8u 24u 80% 8u "Create rexxhide.exe file association"
  Pop $Associate_rexxhide_CK

  ${NSD_SetState} $Associate_rexxhide_CK $Associate_rexxhide_CK_state

  ; Extension line
  ${NSD_CreateLabel}     16u  38u   75u  8u "Extension (no spaces):"
  Pop $0  ; Discarded

  ${NSD_CreateText}      91u  36u  36u 12u $RexxHide_ext_text
  Pop $RexxHide_ext_EDIT

  ; File type line
  ${NSD_CreateLabel}     137u 38u   90u  8u "File type name (no spaces):"
  Pop $0  ; Discarded

  ${NSD_CreateText}      228u 36u  46u 12u $RexxHide_ftype_text
  Pop $RexxHide_ftype_EDIT


  /* Controls for Rexx Paws */
  ${NSD_CreateLabel} 0u 70u 100% 16u \
    "rexxpaws runs a Rexx programs and 'pauses' until the user hits the Enter key, allowing any output to \
    be read before the console window closes."

  Pop $Label_Two

  ${NSD_CreateGroupBox} 0u 92u 100% 44u "File Association for rexxpaws.exe"
  Pop $0  ; Discarded

  ${NSD_CreateCheckBox} 8u 104u 80% 8u "Create rexxpaws.exe file association"
  Pop $Associate_rexxpaws_CK

  ${NSD_SetState} $Associate_rexxpaws_CK $Associate_rexxpaws_CK_state

  ; Extension line
  ${NSD_CreateLabel}     16u  118u  75u  8u "Extension (no spaces):"
  Pop $0  ; Discarded

  ${NSD_CreateText}      91u  116u  36u 12u $RexxPaws_ext_text
  Pop $RexxPaws_ext_EDIT

  ; File type line
  ${NSD_CreateLabel}     137u 118u  90u  8u "File type name (no spaces):"
  Pop $0  ; Discarded

  ${NSD_CreateText}      228u 116u  46u 12u $RexxPaws_ftype_text
  Pop $RexxPaws_ftype_EDIT

  ${If} $DoUpgrade == 'true'
    EnableWindow $Associate_rexxhide_CK 0
	  EnableWindow $RexxHide_ext_EDIT 0
	  EnableWindow $RexxHide_ftype_EDIT 0

  	EnableWindow $Associate_rexxpaws_CK 0
	  EnableWindow $RexxPaws_ext_EDIT 0
	  EnableWindow $RexxPaws_ftype_EDIT 0

    EnableWindow $Label_One 0
    EnableWindow $Label_Two 0

    Call PageDisableQuit
  ${else}
    ${If} $Associate_rexxhide_CK_state == ${BST_UNCHECKED}
  	  EnableWindow $RexxHide_ext_EDIT 0
  	  EnableWindow $RexxHide_ftype_EDIT 0
  	${EndIf}

    ${If} $Associate_rexxpaws_CK_state == ${BST_UNCHECKED}
  	  EnableWindow $RexxPaws_ext_EDIT 0
  	  EnableWindow $RexxPaws_ftype_EDIT 0
    ${EndIf}
  ${EndIf}

  ${NSD_OnClick} $Associate_rexxhide_CK EnableRexxHideAssociation
  ${NSD_OnClick} $Associate_rexxpaws_CK EnableRexxPawsAssociation

  nsDialogs::Show

FunctionEnd

/** Associate_otherExes_leave()  Call back function.
 *
 * This function is called when the RexxHide association page is left.  We check
 * that the text fields are 'sane' and return the user to the page if they
 * are not.  If the text field values are good, the state of the dialog contorls
 * is saved.
 */
Function Associate_otherExes_leave

  /* Check the Rexx Hide controls */
  ${NSD_GetState} $Associate_rexxhide_CK $Associate_rexxhide_CK_state

  ${If} $Associate_rexxhide_CK_state == ${BST_CHECKED}
    ${NSD_GetText} $RexxHide_ext_EDIT $0
    ${NSD_GetText} $RexxHide_ftype_EDIT $1

    ; If either the extension field or the ftype field are blank, abort.
    ${If} $0 == ""
    ${orif} $1 == ""
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Creating a file association requires that$\n\
        both the file extension and the file type field$\n\
        be filed in."

      ${If} $0 == ""
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxHide_ext_EDIT 1
      ${else}
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxHide_ftype_EDIT 1
      ${EndIf}
      Abort
    ${EndIf}

    ; Copy the first char of the ext field to $0 if not a '.', abort.
    StrCpy $2 $0 1
    ${If} $2 != "."
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Please include the leading dot '.' of the file$\n\
        extension."
      SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxHide_ext_EDIT 1
      Abort
    ${EndIf}

    ; Check that the user did not include any spaces in either field. If so abort
    push $0
    call CheckForSpaces
    pop $2
    push $1
    call CheckForSpaces
    pop $3

    ${If} $2 > 0
    ${orif} $3 > 0
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Neither the file extension field nor the file$\n\
        type name field can contain a space."

      ${If} $2 > 0
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxHide_ext_EDIT 1
      ${else}
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxHide_ftype_EDIT 1
      ${EndIf}
      Abort
    ${EndIf}


  ${EndIf}

  /* Check the Rexx Paws controls */
  ${NSD_GetState} $Associate_rexxpaws_CK $Associate_rexxpaws_CK_state

  ${If} $Associate_rexxpaws_CK_state == ${BST_CHECKED}
    ${NSD_GetText} $RexxPaws_ext_EDIT $0
    ${NSD_GetText} $RexxPaws_ftype_EDIT $1

    ; If either the extension field or the ftype field are blank, abort.
    ${If} $0 == ""
    ${orif} $1 == ""
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Creating a file association requires that$\n\
        both the file extension and the file type field$\n\
        be filed in."

      ${If} $0 == ""
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxPaws_ext_EDIT 1
      ${else}
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxPaws_ftype_EDIT 1
      ${EndIf}
      Abort
    ${EndIf}

    ; Copy the first char of the ext field to $0 if not a '.', abort.
    StrCpy $2 $0 1
    ${If} $2 != "."
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Please include the leading dot '.' of the file$\n\
        extension."
      SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxPaws_ext_EDIT 1
      Abort
    ${EndIf}

    ; Check that the user did not include any spaces in either field. If so abort
    push $0
    call CheckForSpaces
    pop $2
    push $1
    call CheckForSpaces
    pop $3

    ${If} $2 > 0
    ${orif} $3 > 0
      MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Neither the file extension field nor the file$\n\
        type name field can contain a space."

      ${If} $2 > 0
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxPaws_ext_EDIT 1
      ${else}
        SendMessage $Dialog ${WM_NEXTDLGCTL} $RexxPaws_ftype_EDIT 1
      ${EndIf}
      Abort
    ${EndIf}

  ${EndIf}

  ; Okay text fields are okay.
  ${NSD_GetText} $RexxHide_ext_EDIT $RexxHide_ext_text
  ${NSD_GetText} $RexxHide_ftype_EDIT $RexxHide_ftype_text
  ${NSD_GetText} $RexxPaws_ext_EDIT $RexxPaws_ext_text
  ${NSD_GetText} $RexxPaws_ftype_EDIT $RexxPaws_ftype_text

FunctionEnd

/** EnableRexxHideAssociation()
 *
 * Called when the user clicks on the Create rexxhide.exe file association check
 * box.  Disables, or enables, the controls depending on if the check box is
 * checked or not.
 */
Function EnableRexxHideAssociation
	Pop $Associate_rexxhide_CK
	${NSD_GetState} $Associate_rexxhide_CK $0

	${If} $0 == 1
	  EnableWindow $RexxHide_ext_EDIT 1
	  EnableWindow $RexxHide_ftype_EDIT 1
	${Else}
	  EnableWindow $RexxHide_ext_EDIT 0
	  EnableWindow $RexxHide_ftype_EDIT 0
	${EndIf}
FunctionEnd

/** EnableRexxPawsAssociation()
 *
 * Called when the user clicks on the Create rexxpaws.exe file association check
 * box.  Disables, or enables, the controls depending on if the check box is
 * checked or not.
 */
Function EnableRexxPawsAssociation
	Pop $Associate_rexxpaws_CK
	${NSD_GetState} $Associate_rexxpaws_CK $0

	${If} $0 == 1
	  EnableWindow $RexxPaws_ext_EDIT 1
	  EnableWindow $RexxPaws_ftype_EDIT 1
	${Else}
	  EnableWindow $RexxPaws_ext_EDIT 0
	  EnableWindow $RexxPaws_ftype_EDIT 0
	${EndIf}
FunctionEnd

/** Confirm_page()  Custom page function.
 *
 * Gives the user one last chance to go back and change any settings.
 *
 * The page is skipped if we are doing an upgrade type of install.
 */
Function Confirm_page

  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
    !insertmacro MUI_HEADER_TEXT "${LONGNAME} is ready for installation." \
                                 "Installation options remain the same for an upgrade type of installation \
                                 of ${LONGNAME}."
  ${else}
    !insertmacro MUI_HEADER_TEXT "${LONGNAME} is ready for installation." \
                                 "All options for ${LONGNAME} have been collected."
  ${EndIf}

  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${If} $DoUpgrade == 'true'
    ${NSD_CreateLabel} 0u 0u 100% 72u \
    "Click the Install button to begin installation.  Click the Back button if you \
     wish to review the installation options.  An upgrade type of installation can \
     not be canceled at this point."
  ${else}
    ${NSD_CreateLabel} 0u 0u 100% 72u \
    "All the parameters needed to install ${LONGNAME} on your system have been \
    gathered together.$\n$\n\
    Click the Install button to begin installation.  Click the Back button to \
    review or change any settings.  Click the Cancel button to abort the installation \
    altogether."
  ${EndIf}

  Pop $Label_One

  ${If} $DoUpgrade == 'true'
    Call PageDisableQuit
  ${EndIf}

  nsDialogs::Show

FunctionEnd

/** PageDisableQuit()
 *
 * Prevents the user from closing (quitting) the installer from the current
 * page.  The function could be generic, but at this point only works if we
 * are doing an upgrade install.
 */
Function PageDisableQuit

  ; Really this should only be called when DoUpgrade is true, but we'll use
  ; a little defensive programming.
  ${If} $DoUpgrade == 'true'
    ; Disable the close button on title bar.
    push $1
    System::Call "user32::GetSystemMenu(i $HWNDPARENT,i 0) i.s"
    pop $1
    System::Call "user32::EnableMenuItem(i $1,i 0xF060,i 1)"
    pop $1

    ; Disable Cancel button
    GetDlgItem $0 $HWNDPARENT 2
    EnableWindow $0 0

    ; Set focus to the Next button
    GetDlgItem $0 $HWNDPARENT 1
    SendMessage $HWNDPARENT ${WM_NEXTDLGCTL} $0 1
  ${EndIf}

FunctionEnd

/** .onMouseOverSection()  Call back function
 *
 * Invoked when the user puts the mouse over one of the componets that can be
 * installed.  This is what provides the description of each component.
 */
Function .onMouseOverSection

  !insertmacro MUI_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "Installs the core components of ${LONGNAME} to the application folder."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDev} "Installs the files required to embed ${LONGNAME} into you C/C++ application."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDemo} "Install sample ${LONGNAME} programs."
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDoc} "Install ${LONGNAME} documentation."
 !insertmacro MUI_DESCRIPTION_END

FunctionEnd

/** DoSendToItems()
 *
 * Create 'Send To' items depending on what the user specified.  This is only
 * called when we are not doing an upgrade type of install.
 *
 * Note that we do not add the shortcut files to the uninstall log.  Deleting
 * the files is done separately from the uninstall log usage because there is
 * no (easy) way to tell which file is being deleted when using the uninstall
 * log.
 */
Function DoSendToItems

  ; Save the user specified settings into the registry variables so the are
  ; written to the registry correctly.
  StrCpy $RegVal_sendTo_rexx $SendTo_rexx_CK_state
  StrCpy $RegVal_sendTo_rexxHide $SendTo_rexxHide_CK_state
  StrCpy $RegVal_sendTo_rexxPaws $SendTo_rexxPaws_CK_state

  SetOutPath $INSTDIR
  ${If} $SendTo_rexx_CK_state == ${BST_CHECKED}
    CreateShortCut "$SENDTO\ooRexx.lnk" "$INSTDIR\rexx.exe" "" "" "" SW_SHOWNORMAL "" "ooRexx"
    DetailPrint "Created Send To rexx.exe item"
  ${EndIf}

  ${If} $SendTo_rexxHide_CK_state == ${BST_CHECKED}
    CreateShortCut "$SENDTO\ooRexx with no console (rexxhide).lnk" "$INSTDIR\rexxhide.exe" "" "" "" SW_SHOWNORMAL "" "ooRexx with no console (rexxhide)"
    DetailPrint "Created Send To rexxhide.exe item"
  ${EndIf}

  ${If} $SendTo_rexxPaws_CK_state == ${BST_CHECKED}
    CreateShortCut "$SENDTO\ooRexx with pause (rexxpaws).lnk" "$INSTDIR\rexxpaws.exe" "" "" "" SW_SHOWNORMAL "" "ooRexx with pause (rexxpaws)"
    DetailPrint "Created Send To rexxpaws.exe item"
  ${EndIf}

FunctionEnd

/** DoFileAssociations()
 *
 * Does the file associations for ooRexx.  Only called when not doing an
 * upgrade.
 *
 * We do two things:
 *   Fill in the values stored in the uninstall section for ooRexx.  These
 *   values tell the uninstaller, when it runs, what it should delete from the
 *   registry.
 *
 *   Write out the actual file associations selected by the user to the
 *   registry.
 */
Function DoFileAssociations

  ; Set everything to blank, then fill in those values the user specified.
  StrCpy $RegVal_rexxAssociation ""
  StrCpy $RegVal_rexxEditor ""
  StrCpy $RegVal_rexxHideAssociation ""
  StrCpy $RegVal_rexxPawsAssociation ""

  ${If} $Use_File_Associations_CK_state == ${BST_UNCHECKED}
    DetailPrint "Do not use Windows file associations specified."
    Return
  ${EndIf}

  ${If} $Associate_rexx_CK_state == ${BST_CHECKED}
    StrCpy $AssociationProgramName 'rexx.exe'
    StrCpy $RegVal_rexxAssociation '$Rexx_ext_text $Rexx_ftype_text'
    StrCpy $RegVal_rexxEditor '$Rexx_editor_text'

    Call AssociateExtensionWithExe
  ${EndIf}

  ${If} $Associate_rexxhide_CK_state == ${BST_CHECKED}
    StrCpy $AssociationProgramName 'rexxhide.exe'
    StrCpy $RegVal_rexxHideAssociation '$RexxHide_ext_text $RexxHide_ftype_text'
    StrCpy $RegVal_rexxEditor '$Rexx_editor_text'

    Call AssociateExtensionWithExe
  ${EndIf}

  ${If} $Associate_rexxpaws_CK_state == ${BST_CHECKED}
    StrCpy $AssociationProgramName 'rexxpaws.exe'
    StrCpy $RegVal_rexxPawsAssociation '$RexxPaws_ext_text $RexxPaws_ftype_text'
    StrCpy $RegVal_rexxEditor '$Rexx_editor_text'

    Call AssociateExtensionWithExe
  ${EndIf}

  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, i 0, i 0)'
FunctionEnd

/** DoEnvVariables()
 *
 * Sets up the variable environemnt variables.
 */
Function DoEnvVariables

    Push "REXX_HOME"
    Push $INSTDIR
    Push $IsAdminUser ; "true" or "false"
    Call WriteEnvStr

    ; Add the install directory to the PATH env variable, either system wide or
    ; user-specific
    Push $INSTDIR
    Push $IsAdminUser ; "true" or "false"
    Push "PATH"
    Call AddToPath

    ; Do the PATHEXT extensions
    Call AddToPathExt
FunctionEnd

/** AssociateExtensionWithExe()
 *
 * Associates a file extension and file type with one of the ooRexx executables,
 * rexx.exe, rexxhide.exe, or rexxpaws.  We are only called when we have
 * something to do.
 *
 * TODO - I added the section to install into the user customizable area some
 *        years ago.  Really what we should be doing, rather than this IsAdmin
 *        stuff is deciding if this is a machine-wide install (All Users) or
 *        a single-user install.  Then use SHCTX or SHELL_CONTEXT to just write
 *        once. SHCTX will use HKCR if it is an all users install and HKCU for
 *        a single user install.
 *
 */
Function AssociateExtensionWithExe

  ; We will put the file extension in $0, the ftype in $1, and the editor file
  ; name in $2
  ${If} $AssociationProgramName == 'rexx.exe'
    ${StrTok} $0 $RegVal_rexxAssociation " " "0" "0"
    ${StrTok} $1 $RegVal_rexxAssociation " " "1" "0"
    StrCpy $AssociationText "ooRexx Rexx Program"
    StrCpy $AssociationEditor '$RegVal_rexxEditor'
  ${elseif} $AssociationProgramName == 'rexxhide.exe'
    ${StrTok} $0 $RegVal_rexxHideAssociation " " "0" "0"
    ${StrTok} $1 $RegVal_rexxHideAssociation " " "1" "0"
    StrCpy $AssociationText "ooRexx Rexx GUI Program"
    StrCpy $AssociationEditor '$RegVal_rexxEditor'
  ${elseif} $AssociationProgramName == 'rexxpaws.exe'
    ${StrTok} $0 $RegVal_rexxPawsAssociation " " "0" "0"
    ${StrTok} $1 $RegVal_rexxPawsAssociation " " "1" "0"
    StrCpy $AssociationText "ooRexx Rexx pausing Program"
    StrCpy $AssociationEditor "$RegVal_rexxEditor"
  ${else}
    DetailPrint "Unrecoverable ERROR. The ftype association executable: $AssociationProgramName is not recognized."
    Goto DoneWithRexxExe
  ${EndIf}

  ${If} $0 == ''
  ${orif} $1 == ''
    DetailPrint "Unrecoverable ERROR.  Empty string in $AssociationProgramName association"
    DetailPrint "  Word one=<$0> word two=<$1>"
    Goto DoneWithRexxExe
  ${EndIf}

  ClearErrors
  ; Write key $0 (.rex for example) with default value of $1 (RexxScript for example.)
  WriteRegStr HKCR $0 "" $1
  IfErrors TryCurrentUser
  DetailPrint "Registered $0 extension as ftype $1 to open with $AssociationProgramName for all users"
  Goto WriteHKCRRegKeys

  TryCurrentUser:
  ; Failed to write to the registy, try the user customizable area.
  ClearErrors
  WriteRegStr HKCU $0 "" $1
  IfErrors 0
    DetailPrint "Could NOT associate $0 extension with $AssociationProgramName for either all users or the current user"
    Goto DonewithRexxExe

  DetailPrint "Registered $0 extension as ftype $1 to open with $AssociationProgramName for the current user."
  WriteRegStr HKCU "SOFTWARE\Classes\$1" "" $AssociationText
  WriteRegStr HKCU "SOFTWARE\Classes\$1\shell" "" "open"
  WriteRegStr HKCU "SOFTWARE\Classes\$1\DefaultIcon" "" "$INSTDIR\$AssociationProgramName,0"
  WriteRegStr HKCU "SOFTWARE\Classes\$1\shell\open" "" "Run"
  WriteRegStr HKCU "SOFTWARE\Classes\$1\shell\open\command" "" '"$INSTDIR\$AssociationProgramName" "%1" %*'

  ${If} $AssociationEditor != ""
    WriteRegStr HKCU "SOFTWARE\Classes\$1\shell\edit" "" "Edit"
    WriteRegStr HKCU "SOFTWARE\Classes\$1\shell\edit\command" "" '"$AssociationEditor" "%1"'
  ${EndIf}

  WriteRegStr HKCU "SOFTWARE\Classes\$1\shellex\DropHandler" "" "{60254CA5-953B-11CF-8C96-00AA00B8708C}"
  Goto DoneWithRexxExe

  WriteHKCRRegKeys:
  WriteRegStr HKCR "$1" "" $AssociationText
  WriteRegStr HKCR "$1\shell" "" "open"
  WriteRegStr HKCR "$1\DefaultIcon" "" "$INSTDIR\$AssociationProgramName,0"
  WriteRegStr HKCR "$1\shell\open" "" "Run"
  WriteRegStr HKCR "$1\shell\open\command" "" '"$INSTDIR\$AssociationProgramName" "%1" %*'

  ${If} $AssociationEditor != ""
    WriteRegStr HKCR "$1\shell\edit" "" "Edit"
    WriteRegStr HKCR "$1\shell\edit\command" "" '"$AssociationEditor" "%1"'
  ${EndIf}

  WriteRegStr HKCR "$1\shellex\DropHandler" "" "{60254CA5-953B-11CF-8C96-00AA00B8708C}"

  DoneWithRexxExe:
FunctionEnd

/** SetDefaultFileAssociation()
 *
 * Use to set the variable values for all the file association controls to the
 * default.
 */
Function SetDefaultFileAssociation
  StrCpy $Use_File_Associations_CK_state '${BST_CHECKED}'

  StrCpy $Associate_rexx_CK_state '${BST_CHECKED}'
  StrCpy $Rexx_ext_text ${DefRexxExt}
  StrCpy $Rexx_ftype_text ${DefRexxFType}
  StrCpy $Rexx_editor_text "$WINDIR\System32\NotePad.exe"

  StrCpy $Associate_rexxhide_CK_state '${BST_UNCHECKED}'
  StrCpy $RexxHide_ext_text ${DefRexxHideExt}
  StrCpy $RexxHide_ftype_text ${DefRexxHideFType}

  StrCpy $Associate_rexxpaws_CK_state '${BST_UNCHECKED}'
  StrCpy $RexxPaws_ext_text ${DefRexxPawsExt}
  StrCpy $RexxPaws_ftype_text ${DefRexxPawsFType}

FunctionEnd

/** SetBlankFileAssociation()
 *
 * Use to set the variable values for all the file association controls to the
 * the empty string.
 */
Function SetBlankFileAssociation
  StrCpy $Use_File_Associations_CK_state '${BST_UNCHECKED}'

  StrCpy $Associate_rexx_CK_state '${BST_UNCHECKED}'
  StrCpy $Rexx_ext_text ""
  StrCpy $Rexx_ftype_text ""
  StrCpy $Rexx_editor_text ""

  StrCpy $Associate_rexxhide_CK_state '${BST_UNCHECKED}'
  StrCpy $RexxHide_ext_text ""
  StrCpy $RexxHide_ftype_text ""

  StrCpy $Associate_rexxpaws_CK_state '${BST_UNCHECKED}'
  StrCpy $RexxPaws_ext_text ""
  StrCpy $RexxPaws_ftype_text ""
FunctionEnd

/** SetFileAssociationVars()
 *
 * Used to set the variable values for the file association dialog controls to
 * the proper value on start up of the installer.
 *
 * Note: we can not know at this time, if the user will elect to do an upgrade
 * type of install.  So the RegVal_* variables can not be changed.
 *
 * If there is no previous install and the reg values are blank, all the
 * variables are set to the defaults.  Likewise, if there is a previous version
 * installed, but the uninstaller version is not able to do an upgrade type of
 * install, we also set the variables to their defaults.
 *
 * Otherwise, the variables are set initially to what is in the registry.
 */
Function SetFileAssociationVars

    ${If} $PreviousVersionInstalled == 'false'
    ${andif} $RegVal_rexxAssociation == ''
      Call SetDefaultFileAssociation
    ${elseif} $UpgradeTypeAvailable == 'false'
      Call SetDefaultFileAssociation
    ${else}
      ${If} $RegVal_rexxAssociation == ''
      ${andif} $RegVal_rexxHideAssociation == ''
      ${andif} $RegVal_rexxPawsAssociation == ''
        Call SetBlankFileAssociation
      ${else}
        ; We will put the file extension in $0 and the ftype in $1

        StrCpy $Use_File_Associations_CK_state '${BST_CHECKED}'
        StrCpy $Rexx_editor_text "$RegVal_rexxEditor"

        ${If} $RegVal_rexxAssociation != ''
          ${StrTok} $0 $RegVal_rexxAssociation " " "0" "0"
          ${StrTok} $1 $RegVal_rexxAssociation " " "1" "0"

          StrCpy $Associate_rexx_CK_state '${BST_CHECKED}'
          StrCpy $Rexx_ext_text $0
          StrCpy $Rexx_ftype_text $1
        ${else}
          StrCpy $Associate_rexx_CK_state '${BST_UNCHECKED}'
        ${EndIf}

        ${If} $RegVal_rexxHideAssociation != ''
          ${StrTok} $0 $RegVal_rexxHideAssociation " " "0" "0"
          ${StrTok} $1 $RegVal_rexxHideAssociation " " "1" "0"

          StrCpy $Associate_rexxhide_CK_state '${BST_CHECKED}'
          StrCpy $RexxHide_ext_text $0
          StrCpy $RexxHide_ftype_text $1
        ${else}
          StrCpy $Associate_rexxhide_CK_state '${BST_UNCHECKED}'
        ${EndIf}

        ${If} $RegVal_rexxPawsAssociation != ''
          ${StrTok} $0 $RegVal_rexxPawsAssociation " " "0" "0"
          ${StrTok} $1 $RegVal_rexxPawsAssociation " " "1" "0"

          StrCpy $Associate_rexxpaws_CK_state '${BST_CHECKED}'
          StrCpy $RexxPaws_ext_text $0
          StrCpy $RexxPaws_ftype_text $1
        ${else}
          StrCpy $Associate_rexxpaws_CK_state '${BST_UNCHECKED}'
        ${EndIf}
      ${EndIf}
    ${EndIf}

FunctionEnd

/** SetSendToVars()
 *
 * Sets the values of the SendTo check box states on start up.  Either to the
 * defaults if there is no previous installation or a pre-4.1.0 installation.
 * Otherwise, the variables are set to what we read out of the registry.
 */
Function SetSendToVars

    ${If} $PreviousVersionInstalled == 'false'
    ${orif} $UpgradeTypeAvailable == 'false'
    ${orif} $RegVal_sendTo_rexx == ''
      StrCpy $SendTo_rexx_CK_state ${BST_UNCHECKED}
      StrCpy $SendTo_rexxHide_CK_state ${BST_CHECKED}
      StrCpy $SendTo_rexxPaws_CK_state ${BST_CHECKED}
    ${else}
      StrCpy $SendTo_rexx_CK_state $RegVal_sendTo_rexx
      StrCpy $SendTo_rexxHide_CK_state $RegVal_sendTo_rexxHide
      StrCpy $SendTo_rexxPaws_CK_state $RegVal_sendTo_rexxPaws
    ${EndIf}

FunctionEnd

/** test */

/** CreateDesktopShortCut()
 *
 * This callback function is executed if, on the finish page, the user checks
 * the Create Desktop Icon check box.  The short cut we create is a link to the
 * Open Object Rexx Start Menu folder.
 */
Function CreateDesktopShortcut

    CreateShortcut "$DESKTOP\Open Object Rexx Resources.lnk" "$SMPROGRAMS\${LONGNAME}\" "" "$INSTDIR\rexx.exe"

    Call OpenUninstallLog
    ${AddItem} "$DESKTOP\Open Object Rexx Resources.lnk"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "CreateDesktopIcon" 1
    Call CloseUninstallLog

FunctionEnd

/** CheckInstalledStatus()
 *
 * Helper function used to determine if there is a previous version of ooRexx
 * installed, and if so what level is the uninstaller at.
 */
Function CheckInstalledStatus

  StrCpy $PreviousVersionInstalled 'false'
  StrCpy $UpgradeTypeAvailable 'false'

  ${If} $RegVal_uninstallString != ""
    StrCpy $PreviousVersionInstalled 'true'

    ${If} $RegVal_uninstallLocation == ""
      ; No location in the registry, we'll use the installation directory.
      StrCpy $RegVal_uninstallLocation $INSTDIR
    ${EndIf}

    ${If} $RegVal_uninstallVersion != ""
      ${VersionCompare} $RegVal_uninstallVersion "4.1.0.0" $0

      ; Returned in $0: 0 == versions are equal, 1 == version is greater than 4.1.0.0, 2 == version is less than 4.1.0.0
      ${If} $0 < 2
        StrCpy $UpgradeTypeAvailable 'true'
      ${EndIf}
    ${EndIf}
  ${EndIf}

FunctionEnd


/** CheckLockedFiles()
 *
 * Helper function used to determine if files are locked.  This function is
 * called on init and right before we show the Components page.  The first time
 * rxapi may or may not be running.  The second time is after rxapi should have
 * been stopped.  If any of these files are locked, we know the install will be
 * inconsistent.  If the files are locked we give the user a chance to close
 * them and retry.
 */
Function CheckLockedFiles

  StrCpy $UserRequestAbort 'false'

check_lock_loop:

  StrCpy $KeyFileName '$RegVal_uninstallLocation\${KEYFILE1}'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$RegVal_uninstallLocation\${KEYFILE2}'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  ; Key file 3 is rxapi.exe.  We only check it when we think it should be
  ; stopped and we are about to do an install.
  ${If} $CheckRxApiLock == 'true'
    StrCpy $KeyFileName '$INSTDIR\${KEYFILE3}'
    LockedList::IsFileLocked $KeyFileName
    Pop $R0
    ${If} $R0 == true
      goto rexxIsRunning
    ${EndIf}
  ${EndIf}

  StrCpy $KeyFileName '$RegVal_uninstallLocation\rexxhide.exe'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$RegVal_uninstallLocation\rexxpaws.exe'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$RegVal_uninstallLocation\ooDialog.exe'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$RegVal_uninstallLocation\ooDialog.dll'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  goto done_out

  rexxIsRunning:
      MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST \
        "WARNING.  The file:$\n$\n\
          $keyFileName$\n$\n\
        is locked and can not be updated by the installer.  This indicates$\n\
        that not all Rexx programs have been halted$\n$\n\
        Continuing with the install in this case is known to cause problems.$\n$\n\
        To Retry:$\n$\n\
        Ensure all Rexx programs and processes are halted and then push$\n\
        Retry.$\n$\n\
        To Quit the install and fix the problem:$\n$\n\
        Push Cancel.  Determine which Rexx programs are open and close$\n\
        them.  Ensure no other Rexx programs or processes are open.  Then$\n\
        retry the install." \
        /SD IDCANCEL IDRETRY check_lock_loop

        StrCpy $UserRequestAbort 'true'

done_out:

FunctionEnd


/** CheckBitnessMatch()
 *
 * Checks that the bitness of the ooRexx to be installed matches the bitness of
 * the ooRexx to be uninstalled.
 */
Function CheckBitnessMatch

  StrCpy $UserRequestAbort 'false'

  ${If} $RegVal_uninstallBitness != ""
    ${If} $RegVal_uninstallBitness != ${CPU}
      MessageBox MB_YESNO \
        "WARNING.  You are installing the ${CPU} version of ooRexx.$\n\
        The installed version of ooRexx is the $RegVal_uninstallBitness version.$\n\
        Running the uninstaller from within the installer in this case$\n\
        is known to cause problems, and is not supported.$\n$\n\
        The previous version of ooRexx should be uninstalled separately.  To$\n\
        do this, halt this installation.  Run the uninstall program using$\n\
        the Control Panel, or the Start Menu item under the Open Object$\n\
        Rexx program group.  Then restart this installation program.$\n$\n\
        Do you wish to continue the installation knowing there may be$\n\
        problems with the install?" \
        /SD IDNO IDYES done_return
        StrCpy $UserRequestAbort 'true'
        done_return:
    ${EndIf}
  ${EndIf}

FunctionEnd

/** CheckStrLen()
 *
 * Checks that this installer is built using the long string version of NSIS.
 */
Function CheckStrLen

  StrCpy $UserRequestAbort 'false'

  ; Max string length is 8192 in the long string build

  ${If} ${NSIS_MAX_STRLEN} < 8192
    MessageBox MB_YESNO \
      "WARNING.  The current installer was built using the short$\n\
      string version of NSIS.  There are 2 problems with this:$\n$\n\
      1.)  This indicates this is not an official ooRexx installer.$\n\
      2.)  There is the possiblity that using this installer will$\n\
      delete the PATH on this system.$\n$\n\
      It is not advised that this installer be used to install$\n\
      ooRexx.  If you continue it is at your own risk.$\n$\n\
      Do you wish to continue despite the risk?" \
      /SD IDNO IDYES done_return
      StrCpy $UserRequestAbort 'true'
      done_return:
  ${EndIf}

FunctionEnd


/** CheckForSpaces()
 *
 * This function checks for spaces in a string.  It is taken from the NSIS
 * examples provided on the NSIS site.
 *
 * It should be used this way:
 *
 * push <str>
 * call CheckForSpaces
 * pop $R0
 *
 * $R0 will contain the number of spaces.  When using the LogicLib, a check like
 * this can be done
 *
 * ${If} $R0 > 0
 *   <do something>
 * ${EndIf}
 *
 */
Function CheckForSpaces
   Exch $R0
   Push $R1
   Push $R2
   Push $R3

   StrCpy $R1 -1
   StrCpy $R3 $R0
   StrCpy $R0 0
   loop:
     StrCpy $R2 $R3 1 $R1
     IntOp $R1 $R1 - 1
     StrCmp $R2 "" done
     StrCmp $R2 " " 0 loop
     IntOp $R0 $R0 + 1
   Goto loop
   done:

   Pop $R3
   Pop $R2
   Pop $R1
   Exch $R0
FunctionEnd

/** AddToPathExt()
 *
 * Adds the file extension(s) associated with the ooRexx executables to PATHEXT.
 *
 * We upper case the extension so that it matches what is commonly seen on
 * Windows.
 *
 * @notes - This could be done for a single-user install, but remember that
 *          the user specific PATHEXT *replaces* the system wide PATHEXT, so
 *          we would need to read the system wide value and add the extension
 *          to it and then write it to the user specific PATHEXT.
 */
Function AddToPathExt

  ${If} $IsAdminUser == "false"  ; TODO fix this
    Return
  ${EndIf}

  ${StrTok} $0 $RegVal_rexxAssociation " " "0" "0"
  ${StrCase} $1 $0 "U"
  DetailPrint "Adding the $1 extension to PATHEXT"
  Push $1
  Push $IsAdminUser      ; should only be "true" at this point
  Push "PATHEXT"
  Call AddToPath

  ${StrTok} $0 $RegVal_rexxHideAssociation " " "0" "0"
  ${StrCase} $1 $0 "U"
  DetailPrint "Adding the $1 extension to PATHEXT"
  Push $1
  Push $IsAdminUser      ; should only be "true" at this point
  Push "PATHEXT"
  Call AddToPath

  ${StrTok} $0 $RegVal_rexxPawsAssociation " " "0" "0"
  ${StrCase} $1 $0 "U"
  DetailPrint "Adding the $1 extension to PATHEXT"
  Push $1
  Push $IsAdminUser      ; should only be "true" at this point
  Push "PATHEXT"
  Call AddToPath

FunctionEnd

/** InstallRxapi()
 *
 * Installs rxapi as a service, if the user elected to do so.
 */
Function InstallRxapi

/*
  rxapi no longer runs as a service, so we never install
  ${If} $RxAPIInstallService == 1
    ; User asked to install rxapi as a service.
    DetailPrint "Installing rxapi as a Windows Service"
    nsExec::ExecToLog "$INSTDIR\rxapi /i /s"
    Pop $R0

    ${If} $R0 == 0
      DetailPrint "rxapi successfully installed as a Windows Service"
      Call StartRxapi
    ${else}
      MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST "Failed to install rxapi as a Windows Service: $R0\n" /SD IDOK
    ${EndIf}
  ${EndIf}
 */
FunctionEnd

/** CheckIsRxapiService()
 *
 * Determines if rxapi is installed as a service.  On return the variable
 * RxapiIsService will be set to either 'true' or 'false'
 *
 */
Function CheckIsRxapiService
  services::IsServiceInstalled 'RXAPI'
  Pop $R0

  ${If} $R0 == 'Yes'
    StrCpy $RxapiIsService 'true'
  ${else}
    StrCpy $RxapiIsService 'false'
  ${EndIf}
FunctionEnd

/** CheckIsRxapirunning()
 *
 * Determines if a rxapi.exe process is running.  On return the variable
 * RxapiIsRunning is set to 'true', 'false' or 'unknown N' where N is an error
 * return code.
 *
 * TODO still need to test on Vista / Windows 7
 *
 */
Function CheckIsRxapiRunning
  ${If} $RxapiIsService == 'true'
    services::IsServiceRunning 'RXAPI'
    Pop $R0
    ${If} $R0 == 'Yes'
      StrCpy $RxapiIsRunning 'true'
    ${else}
      StrCpy $RxapiIsRunning 'false'
    ${EndIf}
  ${else}
    ooRexxProcess::findProcess "rxapi.exe"
    Pop $R0
    DetailPrint "ooRexxProcess::findProcess rc: $R0"
    ${If} $R0 == '0'
      StrCpy $RxapiIsRunning 'true'
    ${elseif} $R0 == '1'
      StrCpy $RxapiIsRunning 'false'
    ${else}
      StrCpy $RxapiIsRunning 'unknown $R0'
    ${EndIf}
  ${EndIf}
FunctionEnd

/** StartRxapi()
 *
 * Starts the rxapi service process, if the user elected to do so.
 */
Function StartRxapi

  /*
  ${If} $RxAPIStartService == 1
    ; User asked to start the rxapi service.
    Services::SendServiceCommand 'start' 'RXAPI'
    Pop $R0

    ${If} $R0 == 'Ok'
      DetailPrint "The rxapi service was successfully sent the start command"
    ${else}
      MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST "Failed to start the ooRexx rxapi service: $R0" /SD IDOK
    ${EndIf}
  ${EndIf}

  */
FunctionEnd

/** StopRxapi()
 *
 * Stop the rxapi.exe process.
 *
 * If rxapi is installed as a service, the stop command should stop it.
 * Otherwise, we use killProcess.
 *
 * On return top of stack will contain 'Ok' for success or 'Error'.  If there
 * was an error top of stack minus 1 will contain the error code from
 * ooRexx::killProcess.
 *
 * @remarks  There was a problem I was having with CheckisRxapiRunning saying
 *           rxapi was running when it actually was not.
 *
 *           When a service is sent the stop command it may take some time for
 *           it to report its state to the service control manager.  In addition
 *           the state may first be changed to stop pending.  In both of these
 *           cases CheckIsRxapirunning would report that rxapi is running.  So,
 *           a .3 sleep is added, which hopefully fixes the problem.
 */
Function StopRxapi

  ${If} $RxapiIsService == 'true'
    Services::SendServiceCommand 'stop' 'RXAPI'
    Pop $R0
    Sleep 300
  ${else}
    ooRexxProcess::killProcess 'rxapi'
    Pop $R0
  ${EndIf}

  Call CheckIsRxapiRunning
  ${If} $RxapiIsRunning == 'false'
    Push 'Ok'
    return
  ${EndIf}

  ; Still running, try one more time, it may be that the service has a stop
  ; pending.  Otherwise, this is probably a waste of time.  But, we will capture
  ; the error code for debugging.  Note that if the return from killProcess is
  ; 1, then the process was not found, so it is not running.  See the @remarks
  ; above.
  ooRexxProcess::killProcess 'rxapi'
  Pop $R0
  ${If} $R0 == 0
  ${orif} $R0 == 1
    Push 'Ok'
  ${else}
    Push $R0
    Push 'Error'
  ${EndIf}
FunctionEnd

;===============================================================================
;  Uninstaller portion of oorexx.nsi.
;===============================================================================

;
; Note this:  If the install was done by a non-admin user, the .rex file association may have been written
;             to the HKEY_CURRENT_USER\SOFTWARE\Classes area.  You would think that we need to check for
;             that and specifically delete those keys.  However, testing shows that the current code always
;             removes those keys.  Tested on a number of machines with a number of different users.

;===============================================================================
;  Uninstaller Sections
;===============================================================================

;-------------------------------------------------------------------------------
; Uninstall section
Section "Uninstall"

  /*
   temporarily comment out orxscrpt stuff while it is disabled in the build.

   ; orxscrpt.dll needs to be degistered  NOTE WSH DLL name may have changed.
   !insertmacro Un
   InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$INSTDIR\orxscrpt.dll"
   ;
   ; In the old WSH code, entering the orxscrpt.dll started up the interpreter.
   ; This may no longer be the case.  If it is, rxapi will need to be stopped
   ; again.
   ooRExxProcess::killProc "rxapi.exe"
  */

  /* If we are doing an upgrade type of install, we leave everything as it was. */
  ${If} $DoUpgrade == 'false'
    ${If} $RxapiIsService == 'true'
      Call un.InstallRxapiService
      Pop $R0
      ${If} $R0 != 0
        MessageBox MB_OK|MB_ICONSTOP \
        "Unexpected error removing the rxapi service.$\n$\n\
        The uninstall will abort.  Report this error to$\n\
        the ${SHORTNAME} developers:$\n$\n\
        Uninstall as service failed: $R0" \
        /SD IDOK
        SetErrorLevel 3
        Quit
      ${EndIf}
    ${EndIf}

    ; One last check, see if rxapi.exe is locked.  We do this here and below
    ; because we want to quit, if we are going to, before anything is undone.
    ; Granted, we may have removed rxapi as a service above, but rxapi /i will
    ; let the user reinstall it if nothing else is deleted.
    StrCpy $CheckRxApiLock 'false'
    Call un.CheckLockedFiles
    ${If} $UserRequestAbort == 'true'
      setErrorLevel 5
      Quit
    ${EndIf}

    ; Get rid of the file associations.  Also removes the extension from
    ; PATHEXT.
    Call un.DeleteFileAssociations

    ; remove directory from PATH
    Push $INSTDIR
    Push $IsAdminUser ; pushes "true" or "false"
    Push "PATH"
    Call un.RemoveFromPath

    ; remove the REXX_HOME environment variable
    Push "REXX_HOME"
    Push $IsAdminUser ; "true" or "false"
    Call un.DeleteEnvStr
  ${EndIf}

  ; One last check, see if rxapi.exe is locked.
  ${If} $DoUpgrade == 'true'
    StrCpy $CheckRxApiLock 'false'
    Call un.CheckLockedFiles
    ${If} $UserRequestAbort == 'true'
      setErrorLevel 5
      Quit
    ${EndIf}
  ${EndIf}

  StrCpy $KeyFileName '$INSTDIR\${KEYFILE3}'

  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
      MessageBox MB_OK|MB_ICONSTOP \
      "Unexpected error the file:$\n$\n\
        $KeyFileName$\n$\n\
      is locked. The uninstall will abort.  Report $\n\
      this error to the ${SHORTNAME} developers." \
      /SD IDOK
      SetErrorLevel 3
      Quit
  ${EndIf}

  ; Upgrade or not, we always remove the installation stuff.
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}"
  ;DeleteRegKey HKLM "SOFTWARE\${LONGNAME}"  We don't write anything here, maybe we should?

  Call un.Delete_Installed_Files

SectionEnd

;===============================================================================
;  Uninstaller Functions
;===============================================================================

/** un.onInit()  Callback function.
 *
 *  Called by the uninstaller program before any pages are shown.  We use it to
 *  set up the execution variables.
 *
 */
Function un.onInit

  StrCpy $DoUpgrade 'false'
  StrCpy $DoUpgradeQuick 'false'

  ${GetOptions} "$CMDLINE" "/U="  $R0
  ${If} $R0 == 'upgrade'
    StrCpy $DoUpgrade 'true'
  ${elseif} $R0 == 'upgradeQuick'
    StrCpy $DoUpgrade 'true'
    StrCpy $DoUpgradeQuick 'true'
  ${EndIf}

  StrCpy $DeleteWholeTree 'false'

  ;StrCpy $DeleteWholeTree 'true'  ; TEMP TEMP TEMP always del tree while testing MM */

  ; UnInstall as All Users if an admin
  Call un.IsUserAdmin
  Pop $IsAdminUser
  ${If} $IsAdminUser == "true"
    SetShellVarContext all
  ${EndIf}

  StrCpy $CheckRxApiLock 'false'
  Call un.CheckLockedFiles
  ${If} $UserRequestAbort == 'true'
    setErrorLevel 5
    abort
  ${EndIf}

  ; Read in the file associations done by the installer.
  ReadRegStr $RegVal_rexxAssociation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxAssociation"
  ReadRegStr $RegVal_rexxHideAssociation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxHideAssociation"
  ReadRegStr $RegVal_rexxPawsAssociation HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORTNAME}" "RexxPawsAssociation"

  StrCpy $StopRxAPI_CK_State 1
  StrCpy $InStopRxapiPage 'false'
  Call un.CheckIsRxapiService
  Call un.CheckIsRxapiRunning

FunctionEnd


/** un.Ok_Stop_RxAPI_page()  Custom page function.
 *
 * This is a custom page that follows the Welcome page.  If rxapi.exe is not
 * running the page is skipped.  If rxapi.exe is running, the user is asked for
 * the okay to stop it.  If the user says no, we quit the uninstall.
 */
Function un.Ok_Stop_RxAPI_page
  ${If} $RxapiIsRunning == 'false'
    Abort
  ${EndIf}

  StrCpy $InStopRxapiPage 'true'

  !insertmacro MUI_HEADER_TEXT "The ooRexx rxapi process." "The ${LONGNAME} memory manager (rxapi) is currently running"
  nsDialogs::Create /NOUNLOAD 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateLabel} 0 0 100% 104u \
    "${SHORTNAME} can not be completely uninstalled while rxapi is running.$\n$\n\
    When the Stop rxapi check box is checked, the uninstall will stop rxapi before \
    proceeding.  If the check box is not checked, rxapi will not be stopped.$\n$\n\
    If rxapi is not stopped the uninstall will be canceled.$\n$\n\
    If, and only if, there are Rexx programs running, stopping the memory manager could \
    possibly cause data loss.$\n$\n\
    If you are worried about this, cancel the uninstall at this point by using the cancel \
    button or unchecking the Stop rxapi check box.  Then stop all running Rexx programs, \
    and rerun the uninstall program, (or the installation program.)"

  Pop $Label_One

  ${NSD_CreateCheckBox} 0 108u 100% 15u "Stop rxapi"
  Pop $StopRxAPI_CK
  ${NSD_SetState} $StopRxAPI_CK $StopRxAPI_CK_State

  GetFunctionAddress $0 un.CheckOkToStopRxapiBack
  nsDialogs::OnBack $0

  nsDialogs::Show

FunctionEnd

/** un.Ok_Stop_RxAPI_leave()  Custom Page Call back function.
 *
 * Invoked by the uninstaller when the user clicks Next on the Ok to stop rxapi
 * page.  We check if the user agreed to stopping rxapi.  If not we quit the
 * uninstaller.  Otherwise we stop rxapi.  If we can not stop rxapi, we also
 * quit the uninstaller.
 */
Function un.Ok_Stop_RxAPI_leave

  ${NSD_GetState} $StopRxAPI_CK $StopRxAPI_CK_State

  ${If} $StopRxAPI_CK_State == 0
    SetErrorLevel 4
    Quit
  ${EndIf}

  StrCpy $InStopRxapiPage 'false'

  Call un.StopRxapi
  Pop $R0
  ${If} $R0 == 'Ok'
    Return
  ${EndIf}

  ; rxapi was not stopped, the error code is now at top of stack
  Pop $R0

  MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST \
             "Can not determine conclusively that rxapi is currently stopped.  It may be$\n\
             that you do not have sufficient privileges to uninstall ${SHORTNAME}.$\n\$\n\
             The uninstall will quit.$\n$\n\
             If you believe you do have sufficient privileges to do the uninstall,$\n\
             you can:$\n$\n\
             1.) Manually stop the rxapi process and restart the uninstall.  If rxapi$\n\
             is installed as a service, stop the service.  Otherwise, use the task$\n\
             manager to end the rxapi process.$\n$\n\
             2.) Ask the ${SHORTNAME} developers for help.  Please report this error code$\n\
             to the ${SHORTNAME} developers: killProcess $R0$\n$\n"\
             /SD IDOK
  SetErrorLevel 2
  Quit

FunctionEnd

Function un.CheckOkToStopRxapiBack
  ${NSD_GetState} $StopRxAPI_CK $StopRxAPI_CK_State
  StrCpy $InStopRxapiPage 'false'
FunctionEnd


/** un.CheckLockedFiles()
 *
 * Helper function used to determine if files are locked.  This function is
 * called on init of the uninstaller.  rxapi may or may not be running, so we
 * don't check it.  If any of these files are locked, we know the install will
 * be inconsistent.  If the files are locked we give the user a chance to close
 * them and retry.
 */
Function un.CheckLockedFiles

  StrCpy $UserRequestAbort 'false'

check_lock_loop:

  StrCpy $KeyFileName '$INSTDIR\${KEYFILE1}'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$INSTDIR\${KEYFILE2}'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  ; Key file 3 is rxapi.exe.  We only check it when we think it should be
  ; stopped.
  ${If} $CheckRxApiLock == 'true'
    StrCpy $KeyFileName '$INSTDIR\${KEYFILE3}'
    LockedList::IsFileLocked $KeyFileName
    Pop $R0
    ${If} $R0 == true
      goto rexxIsRunning
    ${EndIf}
  ${EndIf}

  StrCpy $KeyFileName '$INSTDIR\rexxhide.exe'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$INSTDIR\rexxpaws.exe'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$INSTDIR\ooDialog.exe'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  StrCpy $KeyFileName '$INSTDIR\ooDialog.dll'
  LockedList::IsFileLocked $KeyFileName
  Pop $R0
  ${If} $R0 == true
    goto rexxIsRunning
  ${EndIf}

  goto done_out

  rexxIsRunning:
      MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST \
        "WARNING.  The file:$\n$\n\
          $keyFileName$\n$\n\
        is locked and can not be removed by the uninstaller.  This indicates$\n\
        that not all Rexx programs have been halted$\n$\n\
        Continuing with the uninstall in this case is known to cause problems.$\n$\n\
        To Retry:$\n$\n\
        Ensure all Rexx programs and processes are halted and then push$\n\
        Retry.$\n$\n\
        To Quit the uninstall and fix the problem:$\n$\n\
        Push Cancel.  Determine which Rexx programs are open and close$\n\
        them.  Ensure no other Rexx programs or processes are open.  Then$\n\
        retry the uninstall." \
        /SD IDCANCEL IDRETRY check_lock_loop

        StrCpy $UserRequestAbort 'true'

done_out:

FunctionEnd


/** un.Uninstall_By_Log_page()  Custom page function.
 *
 * This is a custom page that follows the page that checks if it is okay to
 * stop rxapi.  Here we check for the uninstall log.  We then give the user the
 * option of simply deleting the ooRexx directory tree (50 times faster on my
 * system) or using the log file to individually delete only the install files.
 *
 * If the uninstall log is missing, the user does not have the option of
 * deleting individual files and directories.
 */
Function un.Uninstall_By_Log_page

  StrCpy $LogFileExists 'false'
  IfFileExists "$INSTDIR\${UninstLog}" 0 +4
    StrCpy $LogFileExists 'true'

  ${If} $LogFileExists == 'false'
    !insertmacro MUI_HEADER_TEXT \
      "${UninstLog} NOT found." \
      "The option of only removing files installed by the prior ooRexx installer is not available."

    StrCpy $0 \
      "Because the ${UninstLog} file is missing, the uninstall process must remove all \
      files in the $INSTDIR directory tree.$\n$\n\
      WARNING: This will remove all folders and files in the $INSTDIR folder, including \
      any folders or files not placed there by the ooRexx installation.$\n$\n\
      If there are any personl folders or files in the $INSTDIR directory tree that need \
      to be saved, please cancel the uninstall, move the files, and restart the uninstall \
      program."
  ${else}
    !insertmacro MUI_HEADER_TEXT \
      "Choose the method for removing installed files." \
      "Delete only installed files or delete entire directory tree?"

    StrCpy $0 \
      "The uninstall program can use an install log to delete only the folders and files \
      placed in the $INSTDIR directory tree by the original installation program.$\n$\n\
      Optionally, the entire $INSTDIR directory tree can be deleted.$\n$\n\
      WARNING: Deleting the entire directory tree will remove all folders and files in the \
      $INSTDIR folder.  This will include any folders or files not placed there by the ooRexx \
      installation."

  ${EndIf}

  ; If the log file exists and we are doing an upgrade type of uninstall, then
  ; we don't show this page.  On the other hand, if we are doing an upgrade, but
  ; the install log is missing, we do show the page so that the user can cancel
  ; here.

  ${If} $DoUpgrade == 'true'
  ${andif} $LogFileExists == 'true'
    Abort
  ${EndIf}

  nsDialogs::Create /NOUNLOAD 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${If} $LogFileExists == 'false'
    ${NSD_CreateLabel} 0 0 100% 80u $0
    Pop $Label_One

    ${NSD_CreateCheckBox} 0 84u 100% 8u "Delete entire directory tree"
    Pop $Delete_ooRexx_Tree_CK
    ${NSD_Check} $Delete_ooRexx_Tree_CK
    EnableWindow $Delete_ooRexx_Tree_CK 0
  ${else}
    ${NSD_CreateLabel} 0 0 100% 80u $0
    Pop $Label_One

    ${NSD_CreateLabel} 0 100u 100% 16u "To DELETE the entire $INSTDIR directory tree, check the check box."
    Pop $Label_Two

    ${NSD_CreateCheckBox} 0 120u 100% 8u "Delete entire directory tree"
    Pop $Delete_ooRexx_Tree_CK
  ${EndIf}

  ; Set focus to the page dialog rather than the installer dialog, set focus to
  ; the check box, and then show the page dialog
  SendMessage $Dialog ${WM_SETFOCUS} $HWNDPARENT 0
  SendMessage $Dialog ${WM_NEXTDLGCTL} $Delete_ooRexx_Tree_CK 1

  nsDialogs::Show

FunctionEnd

/** un.Uninstall_By_Log_leave()  Call back function.
 *
 * Invoked by the uninstaller when the user clicks Next on the uninstall using
 * the log page.
 */
Function un.Uninstall_By_Log_leave

  ${NSD_GetState} $Delete_ooRexx_Tree_CK $0

  ${If} $0 == 1
    StrCpy $DeleteWholeTree 'true'
  ${else}
    StrCpy $DeleteWholeTree 'false'
  ${EndIf}

FunctionEnd

/** un.Confirm_Page_pre()  Call back function.
 *
 * Invoked before the Confirm page, (the page that shows "uninstalling from,")
 * is created.  We use this to not show that page if we are doing a quick
 * upgrade install.
 *
 */
Function un.Confirm_Page_pre
  ${If} $DoUpgrade == 'true'
  ${andif} $DoUpgradeQuick == 'true'
    Abort
  ${EndIf}
FunctionEnd

/** un.onCancelButton()  Call back function
 *
 * Invoked by the uninstaller when the user clicks the cancel button.  We only
 * care if the user was on the ok to stop rxapi page.  If so, we set the error
 * level to 4 to indicate that, if the uninstall was invoked by the installer,
 * we should just quit.
 */
Function un.onCancelButton
  ${If} $InStopRxapiPage == 'true'
    SetErrorLevel 4
  ${EndIf}
FunctionEnd

/** un.Delete_Installed_Files()
 *
 * Deletes the installed files in the manner specified by the user.  Either by
 * using the log file to delete only files installed by the previous
 * installaltion or by simply deleting the whole installation directory.
 */
Function un.Delete_Installed_Files

  ${If} $DeleteWholeTree == 'true'
    DetailPrint "Uninstall files by deleting the $INSTDIR directory tree"
    RMDir /r "$INSTDIR"
    DetailPrint "Removing all Start Menu short cuts by removing the $SMPROGRAMS\${LONGNAME} folder"
    RMDir /r "$SMPROGRAMS\${LONGNAME}"
    Delete "$DESKTOP\Open Object Rexx Resources.lnk"

  ${else}
    DetailPrint "Uninstall files using the install log file"
    Push $R0
    Push $R1
    Push $R2
    SetFileAttributes "$INSTDIR\${UninstLog}" NORMAL
    FileOpen $UninstLog "$INSTDIR\${UninstLog}" r
    StrCpy $R1 0

    GetLineCount:
      ClearErrors
      FileRead $UninstLog $R0
      IntOp $R1 $R1 + 1
      IfErrors 0 GetLineCount

    LoopRead:
      FileSeek $UninstLog 0 SET
      StrCpy $R2 0
      FindLine:
      FileRead $UninstLog $R0
      IntOp $R2 $R2 + 1
      StrCmp $R1 $R2 0 FindLine

      StrCpy $R0 $R0 -2
      IfFileExists "$R0\*.*" 0 +3
        RMDir $R0  #is dir
      Goto +3
      IfFileExists $R0 0 +2
        Delete $R0 #is file

      IntOp $R1 $R1 - 1
      StrCmp $R1 0 LoopDone
      Goto LoopRead
    LoopDone:
    FileClose $UninstLog

    Delete "$INSTDIR\${UninstLog}"
    RMDir "$INSTDIR"
    Pop $R2
    Pop $R1
    Pop $R0
  ${EndIf}

  /*
   * Remove the send to entries unless we are doing an updgrade type. These
   * entries are not put into the uninstall log.  We simply deleted them if we
   * are not doing an upgrade and leave them alone if we are doing an upgrade.
   *
   * If they do not exist, deleting them does not harm, i.e., there is no
   * warning or error rasised.
   */
  ${If} $DoUpgrade == 'false'
    Delete "$SENDTO\ooRexx.lnk"
    Delete "$SENDTO\ooRexx with pause (rexxpaws).lnk"
    Delete "$SENDTO\ooRexx with no console (rexxhide).lnk"
    DetailPrint "Removed 'Send To' items (if any.)"
  ${EndIf}

FunctionEnd

/** un.InstallRxapiService()
 * Removes the rxapi service.  Only called if RxapiIsService is true.
 *
 * The uninstaller always quits right at the beginning if it can not stop the
 * rxapi.  So logically, rxapi should be stopped, and if it isn't, it should be
 * stoppable.
 *
 * Pushes a return code to the top of the stack.  0 for success, otherwise an
 * error code.
 */
Function un.InstallRxapiService
  Call un.CheckIsRxapiRunning
  ${If} $$RxapiIsRunning == 'true'
    Services::SendServiceCommand 'stop' 'rxapi'
    Pop $R0

    ${If} $R0 != 'Ok'
      ; Seems impossible to get here, but if we did, we'll try to kill rxapi.
      ; If that fails we give up.
      DetailPrint "Service Control Manager failed to stop rxapi, forcing termination"
      ooRexxProcess::killProcess 'rxapi'
      Pop $R0
      ${If} $R0 != 0
      ${andif} $R0 != 1
        Push $R0
        Return
      ${EndIf}
    ${EndIf}
  ${EndIf}

  DetailPrint "Uninstalling ooRexx rxapi Service"
  nsExec::ExecToLog "$INSTDIR\rxapi /u /s"
  Pop $R0

  ${If} $R0 != 0
    ; One reason for this could be that rxapi.exe, the file, has been deleted.
    ; We'll try to handle this case by having the service manager delete the
    ; service.  This can be done even if rxapi.exe is missing.
    Services::SendServiceCommand 'delete' 'rxapi'
    Pop $R0
    ${If} $R0 != 'Ok'
      DetailPrint "Failed to uninstall rxapi as a service.  Reason: $R0"
      Push $R0
      Return
    ${EndIf}
  ${EndIf}

  DetailPrint "Uninstalled rxapi as a service"
  Push 0
FunctionEnd

/** un.DeleteFileAssociations()
 *
 * Deletes the file associations set by the installer.
 *
 * @note  Notice that we do the uninstaller differently from the installer.  In
 *        the installer, addToPathExt() determines which extensions to add to
 *        PATHEXT.  Here, un.DeleteFileAssociations decides which extensions to
 *        remove from the PATHEXT.
 *
 * TODO need to test this well. TODO need to determine if this is a per machine
 * install or a per user install!!!!
 */
Function un.DeleteFileAssociations

  ; We will put the file extension in $0 and the ftype in $1

  ${If} $RegVal_rexxAssociation != ''
    ${UnStrTok} $0 $RegVal_rexxAssociation " " "0" "0"
    ${UnStrTok} $1 $RegVal_rexxAssociation " " "1" "0"

    ; Only delete the association if we own it.  In other words, the ftype we
    ; read from the registry should match the ftype we saved in our uninstall
    ; strings.
    ReadRegStr $2 HKCR "$0" ""
    ${If} $1 == $2
      DeleteRegKey HKCR "$0"
      DeleteRegKey HKCR "$1"
      DetailPrint "Deleted file association for $0"

      push $0
      Call un.AddToPathExt
    ${EndIf}
  ${EndIf}

  ${If} $RegVal_rexxHideAssociation != ''
    ${UnStrTok} $0 $RegVal_rexxHideAssociation " " "0" "0"
    ${UnStrTok} $1 $RegVal_rexxHideAssociation " " "1" "0"

    ReadRegStr $2 HKCR "$0" ""
    ${If} $1 == $2
      DeleteRegKey HKCR "$0"
      DeleteRegKey HKCR "$1"
      DetailPrint "Deleted file association for $0"

      push $0
      Call un.AddToPathExt
    ${EndIf}
  ${EndIf}

  ${If} $RegVal_rexxPawsAssociation != ''
    ${UnStrTok} $0 $RegVal_rexxPawsAssociation " " "0" "0"
    ${UnStrTok} $1 $RegVal_rexxPawsAssociation " " "1" "0"

    ReadRegStr $2 HKCR "$0" ""
    ${If} $1 == $2
      DeleteRegKey HKCR "$0"
      DeleteRegKey HKCR "$1"
      DetailPrint "Deleted file association for $0"

      push $0
      Call un.AddToPathExt
    ${EndIf}
  ${EndIf}

  System::Call 'Shell32::SHChangeNotify(i ${SHCNE_ASSOCCHANGED}, i ${SHCNF_IDLIST}, i 0, i 0)'

FunctionEnd

/** un.AddToPathExt()
 *
 * Removes the specified file extension from PATHEXT.  The file extension is
 * pushed on the stack to send it here.
 *
 * @notes - This could be done for a single-user install, but remember that
 *          ... what? - need to think about how to do this for a single user,
 *          could easily hose the system for a user.  I.e., end up with no
 *          programs running from the command line.
 *
 * Usage:
 *   Push <ext>
 *
 * Here:
 *   Pop $R0 -> $R0 now contains the extension to un.add to PATHEXT.  I.e., the
 *              extension to remove.
 */
Function un.AddToPathExt

  Pop $R0

  ${If} $IsAdminUser == "false"
    Return
  ${EndIf}

  DetailPrint "Removing the $R0 extension from PATHEXT."
  Push $R0
  Push $IsAdminUser      ; should only be "true" at this point
  Push "PATHEXT"
  Call un.RemoveFromPath
FunctionEnd

/** un.CheckIsRxapiService()
 *
 * Checks if rxap is installed as a Windows service.  Copies true or false to
 * the variable: RxapiIsService as appropriate.
 */
Function un.CheckIsRxapiService

  services::IsServiceInstalled 'RXAPI'
  Pop $R0
  ${If} $R0 == 'Yes'
    StrCpy $RxapiIsService 'true'
  ${else}
    StrCpy $RxapiIsService 'false'
  ${EndIf}
FunctionEnd

/** un.CheckIsRxapiRunning()
 *
 * Determines if rxapi.exe is running.
 *
 * On return the top of the stack contains 'true', 'false', or 'unknoww <rc>'
 * where the <rc> is the error recturn code from ooRexxProcess::findProcess.
 *
 * TODO still need to test on Vista / Windows 7 as regular user.
 */
Function un.CheckIsRxapiRunning

  ${If} $RxapiIsService == 'true'
    services::IsServiceRunning 'RXAPI'
    Pop $R0
    ${If} $R0 == 'Yes'
      StrCpy $RxapiIsRunning 'true'
    ${else}
      StrCpy $RxapiIsRunning 'false'
    ${EndIf}
  ${else}
    ooRexxProcess::findProcess "rxapi.exe"
    Pop $R0
    DetailPrint "ooRexxProcess::findProcess rc: $R0"
    ${If} $R0 == '0'
      StrCpy $RxapiIsRunning 'true'
    ${elseif} $R0 == '1'
      StrCpy $RxapiIsRunning 'false'
    ${else}
      StrCpy $RxapiIsRunning 'unknown $R0'
    ${EndIf}
  ${EndIf}
FunctionEnd

/** un.StopRxapi()
 *
 * Stop the rxapi.exe process.
 *
 * If rxapi is installed as a service, the stop command should stop it.
 * Otherwise, we use killProcess.
 *
 * On return top of stack will contain 'Ok' for success or 'Error'.  If there
 * was an error top of stack minus 1 will contain the error code from
 * ooRexx::killProcess.
 *
 * @remarks  There was a problem I was having with CheckisRxapiRunning saying
 *           rxapi was running when it actually was not.
 *
 *           When a service is sent the stop command it may take some time for
 *           it to report its state to the service control manager.  In addition
 *           the state may first be changed to stop pending.  In both of these
 *           cases CheckIsRxapirunning would report that rxapi is running.  So,
 *           a .3 sleep is added, which hopefully fixes the problem.
 */
Function un.StopRxapi

  ${If} $RxapiIsService == 'true'
    Services::SendServiceCommand 'stop' 'RXAPI'
    Pop $R0
    Sleep 300
  ${else}
    ooRexxProcess::killProcess 'rxapi'
    Pop $R0
  ${EndIf}

  Call un.CheckIsRxapiRunning
  ${If} $RxapiIsRunning == 'false'
    Push 'Ok'
    return
  ${EndIf}

  ; Still running, try one more time, it may be that the service has a stop
  ; pending.  Otherwise, this is probably a waste of time.  But, we will capture
  ; the error code for debugging.  Note that if the return from killProcess is
  ; 1, then the process was not found, so it is not running.  See the @remarks
  ; above.
  ooRexxProcess::killProcess 'rxapi'
  Pop $R0
  ${If} $R0 == 0
  ${orif} $R0 == 1
    Push 'Ok'
  ${else}
    Push $R0
    Push 'Error'
  ${EndIf}
FunctionEnd

;eof
