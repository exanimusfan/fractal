Mandelbrot explorer
=======

![Fractal Preview](https://i.ibb.co/vLHdGdg/Fractal.png)  

Explanation
-----------

Mandelbrot explorer's goals were to be a fast, smooth framerate and input,
using any resources I could to make it as fast as I can.
This was initially a school project that I wanted to improve.
Also I wanted to see how far I've come since I made it.
One of the goals was to use as few external libraries as possible.
What I ended up using was the Windows API, OpenCL and OpenGL.  
I attempted to use openMP but made my main loop slower(I suspect because of the cores fighting for cache).
I also attempted SSE but didn't find the performance gains / unreadability trade-off worth it.
The readiblity and cleanliness of the code was ment to show how I usually work.
The idea was to make refactoring as easy as possible while being able to read what the code does at any time.


Requirements
-----------

You need one of the following:  
I.You need cl compiler in your command prompt environment.  
II.Microsoft Visual Studio. (I tried it only with 2019).  
III. Any compiler that can x64 and link with Windows libraries.  

Building the project
-----------

To build the project :  

I. Using command-line cl:  
1. Run setup_cl_x64.bat (it tries to find vcvarsall.bat for you and it runs it in x64 mode)  
2. Run build.bat (The binary will be created inside bin\ directory)  

II.Using Visual Studio:  
1. Open fractal.vcxproj  
2. Run  (The binary will be created in x64\Release or x64\Debug)  

III. Using any other compiler with Windows libs  
1. Compile the windows.c file including the lightOCLSDK/include path for CL and the Windows Kit header files.  
2. Link with these libraries : lightOCLSDK/lib/x86_64/OpenCL.lib user32.lib Gdi32.lib opengl32.lib winmm.lib  

Attention : This is a single translation unit build (Unity build). Compiling all *.c files will probably not work.

Controlling the application
-----------

The application takes the mouse click and drag (movement on the x and y axis)
and mouse wheel(zoom in and out) as input.
You could also use a XINPUT compatible controller (through Steam you could add
any controller, as it simulates XINPUT).
To move on the x and y axis you can use the left thumbstick, and for zoom in and out
you can use the right thumbstick.  
1 or 2 KEYS from the keyboard increase the iterations to 3000 and 5000 respectively.  
ESCAPE or B button (Circle for Playstation) on controller exits the application.
