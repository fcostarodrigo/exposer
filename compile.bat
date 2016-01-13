@echo off

gcc src\*.c -lwsock32 -lws2_32 -mwindows -O2 -o bin\exposer.exe

pause
