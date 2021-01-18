/* date = January 2nd 2021 2:43 pm */

#if !defined(FRACTAL_H)

typedef struct
{
    void *Memory;
    int  Width;
    int  Height;
    int  Pitch;
    int  BytesPerPixel;
    int  LowHighResolution;
}              application_offscreen_buffer;

typedef struct
{
    int   SamplesPerSecond;
    int   SampleCount;
    int16 *Samples;
}              game_sound_output_buffer;

#if 0
typedef enum 
{
    SDL_SCANCODE_1, 1UL);
    SDL_SCANCODE_2, 1UL << 1);
    SDL_SCANCODE_3, 1UL << 2);
    SDL_SCANCODE_4, 1UL << 3);
    SDL_SCANCODE_5, 1UL << 4);
    SDL_SCANCODE_6, 1UL << 5);
    SDL_SCANCODE_ESCAPE, 1UL << 6);
    SDL_SCANCODE_R, 1UL << 7);
    SDL_SCANCODE_T, 1UL << 8);
    SDL_SCANCODE_G, 1UL << 9);
    SDL_SCANCODE_H, 1UL << 10);
    SDL_SCANCODE_B, 1UL << 11);
    SDL_SCANCODE_N, 1UL << 12);
    SDL_SCANCODE_A, 1UL << 13);
    SDL_SCANCODE_D, 1UL << 14);
    SDL_SCANCODE_W, 1UL << 15);
    SDL_SCANCODE_S, 1UL << 16);
};
#endif

internal void ApplicationUpdateAndRender(application_offscreen_buffer Buffer, int *keypress);
internal char *load_program_source(const char *filename, char *source);

#define FRACTAL_H
#endif
