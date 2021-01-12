/* date = January 2nd 2021 2:43 pm */

#if !defined(FRACTAL_H)

typedef struct
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
}              application_offscreen_buffer;

typedef struct
{
    void *Buffer;
    int32 SamplesPerSecond;
    int32 ToneHz;
    int16 ToneVolume;
    int32 BytesPerSample;
    f32   WavePeriod;
    int32 BufferSize;
    f32   tSine;
}              application_sound_output;

internal void ApplicationUpdateAndRender(application_offscreen_buffer Buffer,  int BlueOffset, int GreenOffset);

#define FRACTAL_H
#endif //
