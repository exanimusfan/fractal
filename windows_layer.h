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
    int  LowResolution;
}              win32_offscreen_buffer;

typedef struct
{
    int Width;
    int Height;
}              win32_window_dimension;

typedef struct
{
    uint64 KeyPress;
    int32 MouseWheel;
}              win32_input_handle;

#define WINDOWS_LAYER_H
#endif
