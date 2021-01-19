/* date = January 2nd 2021 2:43 pm */

#if !defined(FRACTAL_H)

typedef struct
{
    void *Memory;
    int  Width;
    int  Height;
    int  Pitch;
    int  BytesPerPixel;
}              application_offscreen_buffer;

typedef struct
{
    int   SamplesPerSecond;
    int   SampleCount;
    int16 *Samples;
}              game_sound_output_buffer;

typedef enum 
{
    DEFAULT, //0
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_ESCAPE,
    KEY_R,
    KEY_T,
    KEY_G,
    KEY_H,
    KEY_B,
    KEY_N,
    KEY_A, // 14
    KEY_D, // 15
    KEY_W,
    KEY_S,
    MOUSE_SCROLL_DOWN,
    MOUSE_SCROLL_UP
} my_keys;

internal void ApplicationUpdateAndRender(application_offscreen_buffer Buffer, int *keypress);
internal char *load_program_source(const char *filename, char *source);

#define FRACTAL_H
#endif
