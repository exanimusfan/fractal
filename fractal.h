/* date = January 2nd 2021 2:43 pm */

#if !defined(FRACTAL_H)

typedef struct
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
} application_offscreen_buffer;

internal void ApplicationUpdateAndRender(application_offscreen_buffer Buffer,  int BlueOffset, GreenOffset);

#define FRACTAL_H
#endif //
