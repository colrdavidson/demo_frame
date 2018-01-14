@echo off

SETLOCAL

set TARGET=x86

cl /c test.cpp /Zl /GS- /Oi /Gm- /GR- /O1 /EHa- /fp:fast
..\crinkler.exe opengl32.lib user32.lib gdi32.lib kernel32.lib winmm.lib test.obj /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup /TINYHEADER /COMPMODE:SLOW /OUT:test.exe
del test.obj
