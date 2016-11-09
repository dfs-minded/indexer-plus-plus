!Include 'MUI.nsh'

!define APPNAME "Indexer++ Beta"
!define VERSIONMAJOR 1
!define VERSIONMINOR 0

VIProductVersion "${VERSIONMAJOR}.${VERSIONMINOR}.0.0"

outfile '${APPNAME} Installer.exe'
InstallDir '$PROGRAMFILES\${APPNAME}'

BrandingText " "

RequestExecutionLevel admin

# This will be in the installer/uninstaller's title bar
Name "${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR}"

!define WELCOME_TITLE 'Welcome to the ${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR} setup.'
!define UNWELCOME_TITLE 'Remove ${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR} from your computer.'
!define FINISH_TITLE 'Installation complieted.'
!define UNFINISH_TITLE 'Uninstall was complited successfully.'

#MUI Settings / Icon
!define MUI_ICON IndexerLogo.ico
!define MUI_UNICON IndexerLogo.ico
 
#MUI Settings / Header
!define MUI_HEADERIMAGE
#!define MUI_HEADERIMAGE_BITMAP ladybug-snail-800.bmp

#MUI Settings / Wizard
#!define MUI_WELCOMEFINISHPAGE_BITMAP ladybug-snail-800.bmp
#!define MUI_UNWELCOMEFINISHPAGE_BITMAP ladybug-snail-800.bmp

#MUI_PAGE
!define MUI_WELCOMEPAGE_TITLE '${WELCOME_TITLE}'
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "License.rtf"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE '${FINISH_TITLE}'
!define MUI_FINISHPAGE_RUN "$INSTDIR\${APPNAME}.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Run ${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR}"
!insertmacro MUI_PAGE_FINISH
 
 #MUI_UNPAGE
!define MUI_WELCOMEPAGE_TITLE '${UNWELCOME_TITLE}'
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE '${UNFINISH_TITLE}'
!insertmacro MUI_UNPAGE_FINISH
 
!insertmacro MUI_LANGUAGE "English"

VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Indexer++"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© Anna Krykora"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Windows Files Search Util"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0.0.0"


 # Sign the installer after it has been created.
!finalize 'signInstaller.cmd'

 
 # Custom macro
 !include LogicLib.nsh
  
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
 
 #-----------------------------
 # Sections
 
 # installer
 
 !define ARP "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
 !include "FileFunc.nsh"
 
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd

 section "Bare minimum" Section1
	SectionIn RO
	# Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
	setOutPath $INSTDIR
	# Files added here should be removed by the uninstaller (see section "uninstall")
	file "${APPNAME}.exe"
	file "CLIInterop.dll"
	file "IndexerLogo.ico"
	file "UserSettings.xml"
	file "AddExplorerContextMenu.exe"
	file "icon_v3_2.ico"
	file "vcomp120.dll"
 
	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "${ARP}" "EstimatedSize" "$0"
	
	# Uninstaller - See function un.onInit and section "uninstall" for configuration
	writeUninstaller "$INSTDIR\Uninstall.exe"
 
	# Start Menu
	createDirectory "$SMPROGRAMS\${APPNAME}"
	createShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\IndexerLogo.ico"
	createShortCut "$SMPROGRAMS\${APPNAME}\Uninstall ${APPNAME}.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\IndexerLogo.ico"
	
	# Write uninstall information to the registry
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$INSTDIR\IndexerLogo.ico"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "Anna Krykora"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${VERSIONMAJOR}.${VERSIONMINOR}"
	
SectionEnd
 #----------------------------
 
Section "Autorun on Startup" Section2
;Running a .exe file on Windows Start
!include "MUI.nsh"

	;WriteRegStr HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}" "$INSTDIR\${APPNAME}.exe"
	
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}" "$INSTDIR\${APPNAME}.exe"
SectionEnd

Section "Context Menu Entry" Section3
	ExecWait '"$INSTDIR\AddExplorerContextMenu.exe" /r "$INSTDIR\${APPNAME}.exe"'
	
SectionEnd
 
# Section "Auto-Updater" Section4

# SectionEnd

Section "Desktop Shortcut" Section5
	CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\IndexerLogo.ico"
SectionEnd
 
LangString DESC_Section1 ${LANG_ENGLISH} "Base ${APPNAME} functionality."
LangString DESC_Section2 ${LANG_ENGLISH} "Launch ${APPNAME} on Windows start."
LangString DESC_Section3 ${LANG_ENGLISH} "Explorer context menu entry for ${APPNAME}. Start search in any directory you want in ${APPNAME} from Windows Explorer."
LangString DESC_Section4 ${LANG_ENGLISH} "Keep your ${APPNAME} update. This option installs update module which searches ${APPNAME} update on Internet and installs it for you."
LangString DESC_Section5 ${LANG_ENGLISH} "Creates Desktop Shortcut for you."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Section1} $(DESC_Section1)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section2} $(DESC_Section2)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section3} $(DESC_Section3)
  # !insertmacro MUI_DESCRIPTION_TEXT ${Section4} $(DESC_Section4)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section5} $(DESC_Section5)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
 
 # Uninstaller
 
function un.onInit
	SetShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd

Section "uninstall"
	# Delete Uninstaller And Unistall Registry Entries
	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${APPNAME}"
	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	#DeleteRegKey from autorun
	#DeleteRegKey HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run\${APPNAME}"
	DeleteRegValue HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}"
	DeleteRegValue HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}"
	# Remove Start Menu launcher
	delete "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk"
	delete "$SMPROGRAMS\${APPNAME}\Uninstall ${APPNAME}.lnk"
	delete "$SMPROGRAMS\${APPNAME}\*.*"
	
	# Try to remove the Start Menu folder - this will only happen if it is empty
	rmDir "$SMPROGRAMS\${APPNAME}"
	
	# Delete Start Menu Shortcuts
	rmDir  "$SMPROGRAMS\${APPNAME}"
	delete "$DESKTOP\${APPNAME}.lnk"
	
	ExecWait '"$INSTDIR\AddExplorerContextMenu.exe" /u'
	
	# Remove files
	delete "$INSTDIR\${APPNAME}.exe"
	delete $INSTDIR\CLIInterop.dll
	delete $INSTDIR\IndexerLogo.ico
	delete $INSTDIR\UserSettings.xml
	delete $INSTDIR\IndexerLog.txt
	delete $INSTDIR\IndexerUILog.txt
	delete $INSTDIR\icon_v3_2.ico
	delete $INSTDIR\vcomp120.dll
	delete $INSTDIR\AddExplorerContextMenu.exe
	delete $INSTDIR\AddExplorerContextMenuErrorLog.txt
	delete $INSTDIR\IndexerDebugLog.txt
	delete $INSTDIR\RecordsDB.txt
	
	# Always delete uninstaller as the last action
	delete $INSTDIR\Uninstall.exe
	rmDir '$INSTDIR'
	
SectionEnd