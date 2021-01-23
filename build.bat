@echo off
REM /DProfiling
REM -W4 -WX -wd4201 -wd4996 -wd4100 treat all warnings as errors, except wd codes

cl -W4 -WX -wd4201 -wd4996 -wd4100 -MT -nologo -O2 /FC /Z7 windows.c /Iincludes /IlightOCLSDK/include /INCREMENTAL:NO user32.lib Gdi32.lib lightOCLSDK\lib\x86_64\OpenCL.lib /Fefractal.exe
