copy /y AddExplorerContextMenu\bin\Release\AddExplorerContextMenu.exe	Installer
copy /y CloseRunningApp\bin\Release\CloseRunningApp.exe   				Installer
copy /y AddDirToPathVarialble\bin\Release\AddDirToPathVarialble.exe   	Installer
copy /y IndexerGUI\Icons\IndexerLogo.ico			 					Installer
copy /y IndexerGUI\Icons\icon_v3_2.ico				 					Installer
copy /y CLIInterop\Release\CLIInterop.dll								Installer
copy /y "IndexerGUI\bin\Release\Indexer++.exe"			 				"Installer\Indexer++ Beta.exe"
copy /y IndexerGUI\bin\Release\UserSettings.xml			 				Installer
copy /y ifind\Release\ifind.exe											Installer
copy /y BasicRE2Syntax.txt												Installer
copy /y LICENSE															Installer
copy /y README.md														Installer
xcopy /s/y wingup 														Installer\updater\

set signToolPath="C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe"

set timestampUrl=http://time.certum.pl
set owner="Open Source Developer, Anna Krykora"

rem sign "Indexer++ Beta.exe"
%signToolPath% sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\Indexer++ Beta.exe"
%signToolPath% verify /pa "Installer\Indexer++ Beta.exe"

REM rem sign CLIInterop.dll
%signToolPath%  sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\CLIInterop.dll"
%signToolPath%  verify /pa "Installer\CLIInterop.dll"

REM rem sign AddExplorerContextMenu.exe
%signToolPath%  sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\AddExplorerContextMenu.exe"
%signToolPath%  verify /pa "Installer\AddExplorerContextMenu.exe"

REM rem sign CloseRunningApp.exe
%signToolPath%  sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\CloseRunningApp.exe"
%signToolPath%  verify /pa "Installer\CloseRunningApp.exe"

REM rem sign AddDirToPathVarialble.exe
%signToolPath%  sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\AddDirToPathVarialble.exe"
%signToolPath%  verify /pa "Installer\AddDirToPathVarialble.exe"

REM rem sign ifind.exe
%signToolPath%  sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\ifind.exe"
%signToolPath%  verify /pa "Installer\ifind.exe"

REM rem compile an installer. The Installer signing will be made after the compilation will be finished.
makensis.exe Installer\IndexerInstaller.nsi

%signToolPath%  sign /n %owner% /t %timestampUrl% /fd sha1 /v "Installer\Indexer++ Beta Installer.exe"
%signToolPath%  verify /pa "Installer\Indexer++ Beta Installer.exe"

pause