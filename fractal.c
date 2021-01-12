/*   fractal.c                                                                */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */

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
