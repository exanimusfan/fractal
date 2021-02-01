@echo off
REM -DProfiling
REM -W4 -WX -wd4201 -wd4996 -wd4100 treat all warnings as errors, except wd codes
REM -wd4100 Unreferenced formal parameter (Variable not used from function paramaters)
REM -wd4201 Nonstandard extension used (Nameles struct/union)
REM -wd4996 ???
REM -Qvec-report:2 -Qpar-report:2
REM -arch:AVX2 maybe should be replaced with sse instructions

set CommonCompilerFlags=-nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -IlightOCLSDK/include
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib Gdi32.lib opengl32.lib winmm.lib lightOCLSDK\lib\x86_64\OpenCL.lib
REM Debug Build
REM cl %CommonCompilerFlags%  -MTd -Od -Z7 windows.c -Fefractal.exe -link %CommonLinkerFlags%
REM Release Build
cl %CommonCompilerFlags% -MT -O2 -GL windows.c -Fefractal.exe -link %CommonLinkerFlags%