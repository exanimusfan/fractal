/*   fractal.c                                                                */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */

internal void
GameOutputSound(game_sound_output_buffer *SoundBuffer, int ToneHz)
{
    local_persist f32 tSine;
    int16 ToneVolume = 3000;
    int WavePeriod = SoundBuffer->SamplesPerSecond/ToneHz;

    int16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0;
        SampleIndex < SoundBuffer->SampleCount;
        ++SampleIndex)
    {
        f32 SineValue = sinf(tSine);
        int16 SampleValue = (int16)(SineValue * ToneVolume);
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        tSine += 2.0f*PI32*1.0f/(f32)WavePeriod;
    }
}

internal void
RenderWierdGradient(application_offscreen_buffer Buffer, int BlueOffset, int GreenOffset)
{
    int Height = Buffer.Height;
    int Width = Buffer.Width;
    uint8 *Row = (uint8 *)Buffer.Memory;

    for (int Y = 0; Y < Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int X = 0; X < Width; ++X)
        {
            uint8 Blue = (X + BlueOffset);
            uint8 Green = (Y + GreenOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }
        Row += Buffer.Pitch;
    }
}

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer, int BlueOffset, int GreenOffset)
{
    RenderWierdGradient(Buffer, BlueOffset, GreenOffset);
}
