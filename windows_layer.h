/* date = January 14th 2021 7:55 am */

#ifndef WINDOWS_LAYER_H

typedef struct
{
    BITMAPINFO Info;
    void *Memory;
    int  Width;
    int  Height;
    int  Pitch;
    int  BytesPerPixel;
    int  LowHighResolution;
}              win32_offscreen_buffer;

typedef struct
{
    int Width;
    int Height;
}              win32_window_dimension;

typedef struct
{
    LPDIRECTSOUNDBUFFER Buffer;
    int32               SamplesPerSecond;
    int32               ToneHz;
    int16               ToneVolume;
    int32               BytesPerSample;
    f32                 WavePeriod;
    int32               BufferSize;
    f32                 tSine;
}              win32_sound_output;

#define WINDOWS_LAYER_H
#endif
