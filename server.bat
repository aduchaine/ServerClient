@echo off

echo This should be played at high volume...
ping -n 2 127.0.0.1 > nul
echo 		preferably in a residential area...
ping -n 2 127.0.0.1 > nul

start login_server.exe
ping -n 3 127.0.0.1 > nul

start content_server.exe
ping -n 3 127.0.0.1 > nul

start client.exe

exit


