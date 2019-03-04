!include 'MUI.nsh'

!define APPNAME "Indexer++ Beta"
!define VERSIONMAJOR 0
!define VERSIONMINOR 5

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

!define MUI_ICON IndexerLogo.ico
!define MUI_UNICON IndexerLogo.ico
!define MUI_HEADERIMAGE
#!define MUI_HEADERIMAGE_BITMAP ladybug-snail-800.bmp
#!define MUI_WELCOMEFINISHPAGE_BITMAP ladybug-snail-800.bmp
#!define MUI_UNWELCOMEFINISHPAGE_BITMAP ladybug-snail-800.bmp

!define MUI_WELCOMEPAGE_TITLE '${WELCOME_TITLE}'
!define MUI_WELCOMEPAGE_TEXT "Setup will guide you through the installation of ${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR}. \n\nClick Next to continue."

#MUI_PAGE
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "EULA.rtf"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE '${FINISH_TITLE}'
!define MUI_FINISHPAGE_RUN "$INSTDIR\${APPNAME}.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Run ${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR}"
!insertmacro MUI_PAGE_FINISH
 
 #MUI_UNPAGE
!define MUI_WELCOMEPAGE_TITLE '${UNWELCOME_TITLE}'
!define MUI_WELCOMEPAGE_TEXT "Setup will guide you through the uninstallation of ${APPNAME} ${VERSIONMAJOR}.${VERSIONMINOR}. \n\nClick Uninstall to start the uninstallation."
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE '${UNFINISH_TITLE}'
!insertmacro MUI_UNPAGE_FINISH
 
!insertmacro MUI_LANGUAGE "English"

VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Indexer++"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© Anna Krykora"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Windows Files Search Util"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0.0.4"

 
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

!macro CheckAndCloseIfRunning
	FindWindow $0 "" "${APPNAME}"
	StrCmp $0 0 notRunning
		ExecWait "CloseRunningApp.exe" ;close during new installation. 
		ExecWait "$INSTDIR\CloseRunningApp.exe" ;close on uninstall.
		Sleep 1500 ;Let the application to close
	notRunning:
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

ShowInstDetails show

Section "Bare minimum" Section1
	SectionIn RO
	
	!insertmacro CheckAndCloseIfRunning
	
	# Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
	setOutPath $INSTDIR
	# Files added here should be removed by the uninstaller (see section "uninstall")
	file "${APPNAME}.exe"
	file "CLIInterop.dll"
	file "IndexerLogo.ico"
	file "AddExplorerContextMenu.exe"
	file "CloseRunningApp.exe"
	file "AddDirToPathVarialble.exe"
	file "icon_v3_2.ico"
	file "vcomp120.dll"
	file "ifind.exe"
	file "BasicRE2Syntax.txt"
	file "ScheduleIndexerAsTask.cmd"
	file "DeleteScheduledAsTaskIndexer.cmd"
	file "LICENSE"
	file "README"
	
	setOutPath "$INSTDIR\updater"
	file /nonfatal /a /r "updater\"
 
	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "${ARP}" "EstimatedSize" "$0"
	
	# Uninstaller - See function un.onInit and section "uninstall" for configuration
	writeUninstaller "$INSTDIR\Uninstall ${APPNAME}.exe"
 
	# Start Menu
	createDirectory "$SMPROGRAMS\${APPNAME}"
	createShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\IndexerLogo.ico"
	createShortCut "$SMPROGRAMS\${APPNAME}\Uninstall ${APPNAME}.lnk" "$INSTDIR\Uninstall ${APPNAME}.exe" "" "$INSTDIR\IndexerLogo.ico"
	
	# Write uninstall information to the registry
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\Uninstall ${APPNAME}.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$INSTDIR\IndexerLogo.ico"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "Anna Krykora"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "HelpLink" "http://indexer-plus-plus.com"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${VERSIONMAJOR}.${VERSIONMINOR}"
	
	# (For versions <= 0.4) Delete from autorun in registry
	DeleteRegValue HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}"
	DeleteRegValue HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}"
	DeleteRegValue HKEY_LOCAL_MACHINE "SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}"
	
SectionEnd
 
 
 # The "" makes the section hidden.
Section "" SecUninstallPrevious
	Call UninstallPrevious
SectionEnd

Function UninstallPrevious

	# Check for uninstaller.
	#ReadRegStr $R0 HKLM "${HKLM_REG_KEY}" "InstallDir"
	ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation"
	
	${If} $R0 == ""        
		DetailPrint "No previous versions installation found."    	
		Goto Done
	${EndIf}

	DetailPrint "Removing previous installation."    

	# Run the uninstaller silently.
	ExecWait '"$R0\Uninstall ${APPNAME}.exe /S"'

	Done:

FunctionEnd

Section "Autorun on Startup" Section2
# Schedule to run the App when user logged in
ExpandEnvStrings $0 %COMSPEC%
ExecWait '"$INSTDIR\ScheduleIndexerAsTask.cmd" "$INSTDIR\${APPNAME}.exe"'
SectionEnd

Section "Context Menu Entry" Section3
	ExecWait '"$INSTDIR\AddExplorerContextMenu.exe" /r "$INSTDIR\${APPNAME}.exe"'
SectionEnd

Section "Add to PATH variable for Current User" Section4
	ExecWait '"$INSTDIR\AddDirToPathVarialble.exe" /a "$INSTDIR" /u'
SectionEnd

Section "Add to PATH variable for local machine" Section5
	ExecWait '"$INSTDIR\AddDirToPathVarialble.exe" /a "$INSTDIR" /m'
SectionEnd
 
# Section "Auto-Updater" Section6

# SectionEnd

Section "Desktop Shortcut" Section7
	CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\IndexerLogo.ico"
SectionEnd
 
LangString DESC_Section1 ${LANG_ENGLISH} "Base ${APPNAME} functionality."
LangString DESC_Section2 ${LANG_ENGLISH} "Launch ${APPNAME} on Windows start."
LangString DESC_Section3 ${LANG_ENGLISH} "Explorer context menu entry for ${APPNAME}. Start search in any directory you want in ${APPNAME} from Windows Explorer."
LangString DESC_Section4 ${LANG_ENGLISH} "Adds Indexer++ and ifind to the PATH variable for Current User."
LangString DESC_Section5 ${LANG_ENGLISH} "Adds Indexer++ and ifind to the PATH variable for local machine."
#LangString DESC_Section6 ${LANG_ENGLISH} "Keep your ${APPNAME} update. This option installs update module which searches ${APPNAME} update on Internet and installs it for you."
LangString DESC_Section7 ${LANG_ENGLISH} "Creates Desktop Shortcut for you."

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Section1} $(DESC_Section1)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section2} $(DESC_Section2)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section3} $(DESC_Section3)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section4} $(DESC_Section4)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section5} $(DESC_Section5)
  # !insertmacro MUI_DESCRIPTION_TEXT ${Section6} $(DESC_Section6)
  !insertmacro MUI_DESCRIPTION_TEXT ${Section7} $(DESC_Section7)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
 
 # Uninstaller
 
function un.onInit
	SetShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd

Section "uninstall"

	!insertmacro CheckAndCloseIfRunning
	
	# Delete App from task scheduler
	ExpandEnvStrings $0 %COMSPEC%
	ExecWait '"$INSTDIR\DeleteScheduledAsTaskIndexer.cmd"'
		
	# Delete Uninstaller And Unistall Registry Entries
	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${APPNAME}"
	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"

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
	ExecWait '"$INSTDIR\AddDirToPathVarialble.exe" /r "$INSTDIR" /u'
	ExecWait '"$INSTDIR\AddDirToPathVarialble.exe" /r "$INSTDIR" /m'
	
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
	delete $INSTDIR\AddDirToPathVarialble.exe
	delete $INSTDIR\CloseRunningApp.exe
	delete $INSTDIR\AddExplorerContextMenuErrorLog.txt
	delete $INSTDIR\IndexerDebugLog.txt
	delete $INSTDIR\RecordsDB.txt
	delete $INSTDIR\ifind.exe
	delete $INSTDIR\helpText.txt
	delete $INSTDIR\BasicRE2Syntax.txt
	delete $INSTDIR\ScheduleIndexerAsTask.cmd
	delete $INSTDIR\DeleteScheduledAsTaskIndexer.cmd
	delete $INSTDIR\LICENSE
	delete $INSTDIR\README
	
	rmDir /r /REBOOTOK $INSTDIR\updater
	rmDir /r /REBOOTOK $INSTDIR\Logs
	
	# Always delete uninstaller as the last action
	delete "$INSTDIR\Uninstall ${APPNAME}.exe"
	rmDir /r /REBOOTOK '$INSTDIR'
	
SectionEnd
