rem xcopy /s c:\source d:\target
xcopy /y AddExplorerContextMenu\bin\Release\AddExplorerContextMenu.exe  Installer
xcopy /y IndexerGUI\Icons\IndexerLogo.ico  			 						Installer
xcopy /y IndexerGUI\Icons\icon_v3_2.ico  			 							Installer
xcopy /y CLIInterop\Release\CLIInterop.dll								Installer
xcopy /y IndexerGUI\bin\Release\Indexer++.exe			 				Installer
xcopy /y IndexerGUI\bin\Release\UserSettings.xml			 			Installer

makensis.exe Installer\IndexerInstaller.nsi

"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p Ol7*nF4GnpM#G /d "Indexer++ Beta Installer.exe" "Installer\Indexer++ Beta Installer.exe"

"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Installer\Indexer++ Beta Installer.exe"
pause
