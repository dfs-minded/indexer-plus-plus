setlocal
set runlevel=

REM Get OS version from registry
for /f "tokens=2*" %%i in ('reg.exe query "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion" /v "CurrentVersion"') do set os_ver=%%j

REM Set run level (for Vista or later - version 6)
if /i "%os_ver:~,1%" GEQ "6" set runlevel=/rl HIGHEST

schtasks /create /tn "Indexer++" /sc ONLOGON /tr '%1'" -scheduled" /f /ru %USERNAME% %runlevel% /delay 0000:15 /it