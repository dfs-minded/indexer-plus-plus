rem xcopy /s c:\source d:\target
copy /y AddExplorerContextMenu\bin\Release\AddExplorerContextMenu.exe	Installer
copy /y CloseRunningApp\bin\Release\CloseRunningApp.exe   				Installer
copy /y AddDirToPathVarialble\bin\Release\AddDirToPathVarialble.exe   	Installer
copy /y IndexerGUI\Icons\IndexerLogo.ico			 					Installer
copy /y IndexerGUI\Icons\icon_v3_2.ico				 					Installer
copy /y CLIInterop\Release\CLIInterop.dll								Installer
copy /y "IndexerGUI\bin\Release\Indexer++.exe"			 				"Installer\Indexer++ Beta.exe"
copy /y IndexerGUI\bin\Release\UserSettings.xml			 				Installer
copy /y ifind\Release\ifind.exe											Installer
copy /y ifind\Release\helpText.txt										Installer
copy /y BasicRE2Syntax.txt												Installer


set signToolPath="C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe"

set timestampUrl=http://timestamp.verisign.com/scripts/timstamp.dll

set pass=***

rem sign "Indexer++ Beta.exe"
%signToolPath% sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "Indexer++ Beta.exe" "Installer\Indexer++ Beta.exe"
%signToolPath% verify /pa "Installer\Indexer++ Beta.exe"

rem sign CLIInterop.dll
%signToolPath%  sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "CLIInterop.dll" "Installer\CLIInterop.dll"
%signToolPath%  verify /pa "Installer\CLIInterop.dll"

rem sign AddExplorerContextMenu.exe
%signToolPath%  sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "AddExplorerContextMenu.exe" "Installer\AddExplorerContextMenu.exe"
%signToolPath%  verify /pa "Installer\AddExplorerContextMenu.exe"

rem sign CloseRunningApp.exe
%signToolPath%  sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "CloseRunningApp.exe" "Installer\CloseRunningApp.exe"
%signToolPath%  verify /pa "Installer\CloseRunningApp.exe"

rem sign AddDirToPathVarialble.exe
%signToolPath%  sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "AddDirToPathVarialble.exe" "Installer\AddDirToPathVarialble.exe"
%signToolPath%  verify /pa "Installer\AddDirToPathVarialble.exe"

rem sign ifind.exe
%signToolPath%  sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "ifind.exe" "Installer\ifind.exe"
%signToolPath%  verify /pa "Installer\ifind.exe"

rem compile an installer. The Installer signing will be made after the compilation will be finished.
makensis.exe Installer\IndexerInstaller.nsi

echo Signing Installer after pressing any key. Be sure the installer file is ready to be signed.

pause

%signToolPath%  sign /t %timestampUrl% /f "anna_dev_cert.p12" /p %pass% /d "Indexer++ Beta Installer.exe" "Installer\Indexer++ Beta Installer.exe"
%signToolPath%  verify /pa "Installer\Indexer++ Beta Installer.exe"

pause
