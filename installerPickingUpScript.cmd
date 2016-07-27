rem xcopy /s c:\source d:\target
xcopy /y AddExplorerContextMenu\bin\Release\AddExplorerContextMenu.exe  Installer
xcopy /y IndexerGUI\Icons\IndexerLogo.ico  			 						Installer
xcopy /y IndexerGUI\Icons\icon_v3_2.ico  			 							Installer
xcopy /y CLIInterop\Release\CLIInterop.dll								Installer
xcopy /y IndexerGUI\bin\Release\Indexer++.exe			 				Installer
xcopy /y IndexerGUI\bin\Release\UserSettings.xml			 			Installer

makensis.exe Installer\IndexerInstaller.nsi
pause
