/*   fractal.c                                                                */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */

#include "fractol.h"
#include "srcs/cl_helper.c"
#if 1
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
#endif

internal int 
run_cl(application_offscreen_buffer Buffer, t_fol *fol, int x, int y)
{
	t_args			arg;
	unsigned int	i;
	unsigned int	j;

	arg = init_kernel_args();
	j = 0;
	if (!(fol->flag & (1UL << 4)))
	{
		i = 0;
		get_context(fol, i);
		fol->ocl.image = clCreateBuffer(fol->ocl.context, CL_MEM_WRITE_ONLY,
                                        fol->ocl.buff_size, NULL, &fol->ocl.err);
		check_succeeded("Creating buffer", fol->ocl.err);
		while (j < 9)
		{
			if (fol->frac & 1 << j)
				fol->ocl.krnl = load_krnl(fol->ocl.context, arg.args[j]);
			j++;
		}
	}
	ft_init_args(fol, x, y);
	run_kernel(fol);
	//clFinish(fol->ocl.cmd_queue[0]);
	return (0);
}

#if 0
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
#endif

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer, int BlueOffset, int GreenOffset)
{
    t_fol fol;
    //RenderWierdGradient(Buffer, BlueOffset, GreenOffset);

    fol.flag ^= 1UL;
    fol.flag ^= 1UL << 2;
    fol.k.red = 0.002f;
    fol.k.green = 0.003f;
    fol.k.blue = 0.005f;
    fol.zoom = 1;
    fol.k.xmax = 3.5f * fol.zoom;
    fol.k.xmin = 2.5f * fol.zoom;
    fol.k.ymax = 2.5f * fol.zoom;
    fol.k.ymin = 1.25f * fol.zoom;
    fol.k.iter = 1000;
    fol.x = W / 2;
    fol.y = H / 2;
    fol.k.xoffset = 0.74f;
    fol.k.yoffset = 0;
    fol.keypress = 0;
    fol.accel.x = 0;
    fol.accel.y = 0;
    fol.img = Buffer.Memory;
    run_cl(Buffer, &fol, Buffer.Width, Buffer.Height);
}
