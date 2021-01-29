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
}              win32_offscreen_buffer;

typedef struct
{
    int Width;
    int Height;
}              win32_window_dimension;

typedef struct
{
    void   *Memory;
    size_t UsedSize;
    size_t TotalSize;
}              win32_persistent_storage;

#define WINDOWS_LAYER_H
#endif
