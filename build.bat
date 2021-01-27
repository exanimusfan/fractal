@echo off
REM -DProfiling
REM -W4 -WX -wd4201 -wd4996 -wd4100 treat all warnings as errors, except wd codes
REM -wd4100 Unreferenced formal parameter (Variable not used from function paramaters)
REM -wd4201 Nonstandard extension used (Nameles struct/union)
REM -wd4996 ???

set CommonCompilerFlags=-DProfiling -W4 -WX -wd4100 -wd4201 -MT -nologo -FC
set MyAppLibraries=user32.lib Gdi32.lib opengl32.lib winmm.lib lightOCLSDK\lib\x86_64\OpenCL.lib
REM  Debug Build
REM cl %CommonCompilerFlags%  -Od -Z7 windows.c -Iincludes -IlightOCLSDK/include -INCREMENTAL:NO %MyAppLibraries% /Fefractal.exe
REM Release Build
cl %CommonCompilerFlags% -O2 windows.c -Iincludes -IlightOCLSDK/include -INCREMENTAL:NO %MyAppLibraries% /Fefractal.exe
