Mandelbrot explorer
=======

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

Building the project
-----------

To build the project you either need cl compiler in your command prompt environment
or you can open the project file with Microsoft Visual Studio. (I tried it only with 2019).  
If you don't have cl in your %PATH% environment variable, you can try and run setup_cl_x64.bat
and it will try to find the vcvarsall.bat for you. Next step is to run build.bat  
You could also try and compile windows.c file and add lightOCLSDK/include for header files and
link with lightOCLSDK/lib/x86_64/OpenCL.lib and all the necessary
windows libs(user32.lib Gdi32.lib opengl32.lib winmm.lib)
This is a single translation unit build (Unity build). Compiling all *.c files will probably not work.


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
