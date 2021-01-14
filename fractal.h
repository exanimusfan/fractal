/* date = January 2nd 2021 2:43 pm */

#if !defined(FRACTAL_H)

typedef struct
{
    void *Memory;
    int  Width;
    int  Height;
    int  Pitch;
}              application_offscreen_buffer;

typedef struct
{
    int   SamplesPerSecond;
    int   SampleCount;
    int16 *Samples;
}              game_sound_output_buffer;

internal void ApplicationUpdateAndRender(application_offscreen_buffer Buffer,  int BlueOffset, int GreenOffset);

#define FRACTAL_H
#endif
