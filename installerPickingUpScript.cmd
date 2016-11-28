rem xcopy /s c:\source d:\target
copy /y AddExplorerContextMenu\bin\Release\AddExplorerContextMenu.exe   Installer
copy /y CloseRunningApp\bin\Release\CloseRunningApp.exe   				Installer
copy /y IndexerGUI\Icons\IndexerLogo.ico  			 					Installer
copy /y IndexerGUI\Icons\icon_v3_2.ico  			 					Installer
copy /y CLIInterop\Release\CLIInterop.dll								Installer
copy /y "IndexerGUI\bin\Release\Indexer++.exe"			 				"Installer\Indexer++ Beta.exe"
copy /y IndexerGUI\bin\Release\UserSettings.xml			 				Installer

rem sign "Indexer++ Beta.exe"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p o{P87=bD14Unj /d "Indexer++ Beta.exe" "Installer\Indexer++ Beta.exe"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Installer\Indexer++ Beta.exe"

rem sign CLIInterop.dll
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p o{P87=bD14Unj /d "CLIInterop.dll" "Installer\CLIInterop.dll"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Installer\CLIInterop.dll"

rem sign AddExplorerContextMenu.exe
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p o{P87=bD14Unj /d "AddExplorerContextMenu.exe" "Installer\AddExplorerContextMenu.exe"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Installer\AddExplorerContextMenu.exe"

rem sign CloseRunningApp.exe
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p o{P87=bD14Unj /d "CloseRunningApp.exe" "Installer\CloseRunningApp.exe"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Installer\CloseRunningApp.exe"

rem compile an installer. The Installer signing will be made after the compilation will be finished.
makensis.exe Installer\IndexerInstaller.nsi

echo Signing Installer after pressing any key. Be sure the installer file is ready to be signed.

pause

"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p o{P87=bD14Unj /d "Indexer++ Beta Installer.exe" "Installer\Indexer++ Beta Installer.exe"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Installer\Indexer++ Beta Installer.exe"

pause
