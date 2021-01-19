rem cl -O2 /DProfiling /Zi windows.c user32.lib Gdi32.lib lightOCLSDK\lib\x86_64\OpenCL.lib /Fefractal.exe
cl -O2 /FC /Zi windows.c /Iincludes /IlightOCLSDK/include user32.lib Gdi32.lib lightOCLSDK\lib\x86_64\OpenCL.lib /Fefractal.exe
