rem cl -O2 /DProfiling /Zi windows.c user32.lib Gdi32.lib lightOCLSDK\lib\x86_64\OpenCL.lib /Fefractal.exe
cl -O1 /FC /DProfiling /Zi windows.c /Iincludes /IlightOCLSDK/include /INCREMENTAL:NO user32.lib Gdi32.lib lightOCLSDK\lib\x86_64\OpenCL.lib /Fefractal.exe
