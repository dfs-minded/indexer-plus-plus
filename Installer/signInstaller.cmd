echo ========Signing Installer========
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" sign /t http://timestamp.verisign.com/scripts/timstamp.dll /f "anna_dev_cert.p12" /p strongPass /d "Indexer++ Beta Installer.exe" "Indexer++ Beta Installer.exe"
"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\signtool.exe" verify /pa "Indexer++ Beta Installer.exe"
