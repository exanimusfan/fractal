REM /DProfiling
cl -O1 /DProfiling /FC /Zi windows.c /Iincludes /IlightOCLSDK/include /INCREMENTAL:NO user32.lib Gdi32.lib lightOCLSDK\lib\x86_64\OpenCL.lib /Fefractal.exe
