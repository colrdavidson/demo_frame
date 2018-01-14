@echo off

SETLOCAL

cl test.cpp /DDEBUG /MD /O2 /link opengl32.lib user32.lib gdi32.lib winmm.lib /SUBSYSTEM:CONSOLE /ENTRY:WinMainCRTStartup /NOLOGO
del test.obj
