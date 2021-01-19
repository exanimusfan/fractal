/*   fractal.c                                                                */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */

#include "fractol.h"
#include "srcs/exit.c"
#include "srcs/cl_init.c"
#include "srcs/cl_helper.c"
#include "srcs/keyspress.c"

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

internal cl_kernel
load_krnl(cl_device_id device, cl_context context,
          const char *filename)
{
    size_t	size;
    char	source[4096];
    cl_program	program[1];
    cl_kernel	kernel[1];
    int		err = 0;
    char	*program_source;

    program_source = load_program_source(filename, &source[0]);
    if (program_source != NULL)
	{
        program[0] = clCreateProgramWithSource(context, 1,
                                               (const char**)&program_source, NULL, &err);
        check_succeeded("Loading kernel", err);
		err = clBuildProgram(program[0], 0, NULL, "-I opencl", NULL, NULL);
        err = clGetProgramBuildInfo (program[0], device, CL_PROGRAM_BUILD_LOG,
                                     4096, &source[0],&size);
        DebugOut("%s\nEND\n", source);
        check_succeeded("Building program", err);
		kernel[0] = clCreateKernel(program[0], "render", &err);
    }
	return (kernel[0]);
}

internal int 
run_cl(application_offscreen_buffer Buffer, t_fol *fol)
{
	t_args		arg;
	unsigned int	j;

	arg = init_kernel_args();
	j = 0;
	if (!(fol->flag & (1UL << 4)) || fol->old_buffer_size.x != Buffer.Width || fol->old_buffer_size.y != Buffer.Height)
	{
        DebugOut("Reallocate buffer ? \n");
        fol->old_buffer_size.x = Buffer.Width;
        fol->old_buffer_size.y = Buffer.Height;
        clFinish(fol->ocl.cmd_queue[0]);
        finish_cl(fol->ocl.context, fol->ocl.cmd_queue[0], fol->ocl.image);
        fol->x = Buffer.Width / 2;
        fol->y = Buffer.Height / 2;
        get_context(Buffer, fol, 0);
        fol->ocl.image = clCreateBuffer(fol->ocl.context, CL_MEM_WRITE_ONLY,
                                        fol->ocl.buff_size, NULL, &fol->ocl.err);
        check_succeeded("Creating buffer", fol->ocl.err);
        fol->frac = 1;
        while (j < 9)
		{
			if (fol->frac & 1 << j)
            {
				fol->ocl.krnl = load_krnl(fol->ocl.devices[0], fol->ocl.context, arg.args[j]);
            }
            j++;
		}
	}
	ft_init_args(fol);
	run_kernel(Buffer, fol);
	clFinish(fol->ocl.cmd_queue[0]);
    int MiddleX = Buffer.Width / 2;
    int MiddleY = Buffer.Height / 2;
    int Iterator = 0;

    int *Middle = Buffer.Memory;

    while (Iterator < Buffer.Width)
    {
        Middle[(MiddleY * Buffer.Width) + Iterator] = 0xFFFF0000;
        Iterator++;
    }
    Iterator = 0;
    while (Iterator < Buffer.Height)
    {
        Middle[(Iterator * Buffer.Width) + MiddleX] = 0xFFFF0000;
        Iterator++;
    }
	return (0);
}

//TODO: Implemented the flag system, what remains is enum for better readability

f32	lerp(f32 a, f32 t, f32 b)
{
	f32	result;

	result = (1.0f - t) * a + (t * b);
    return (result);
}

#include <stdio.h>

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer, int *keypress)
{
    local_persist t_fol fol;

    if (Buffer.Memory)
        fol.img = Buffer.Memory;
    fol.keypress = *keypress;
    if (fol.flag == 0)
    {
        float xrange;
        float yrange;

        fol.flag ^= 1UL;
        fol.flag ^= 1UL << 2;
        xrange = 2;
        yrange = xrange / ((float)Buffer.Width / (float)Buffer.Height);
        xrange *= 3;
        yrange *= 3;
        fol.k.red = 0.002f;
        fol.k.green = 0.003f;
        fol.k.blue = 0.005f;
        fol.zoom = 1;
        fol.k.xoffset = -9.04f;
        fol.k.yoffset = -4.84f;
        fol.k.xmin = -xrange;
        fol.k.xmax = xrange;
        fol.k.ymin = -yrange;
        fol.k.ymax = yrange;

        fol.k.iter = 5000;
        fol.x = Buffer.Width / 2;
        fol.y = Buffer.Height / 2;
        fol.keypress = 0;
        fol.accel.x = 0;
        fol.accel.y = 0;
        fol.old_buffer_size.x = Buffer.Width;
        fol.old_buffer_size.y = Buffer.Height;
        run_cl(Buffer, &fol);
    }

    if (fol.keypress != 0)
    {
        check_keypress(&fol);
        if (*keypress & (1UL << 17))
        {
            if (*keypress & (1UL << 18))
            {
                fol.k.xmax *= 0.99f;
                fol.k.xmin *= 0.99f;
                fol.k.ymax *= 0.99f;
                fol.k.ymin *= 0.99f;
                fol.k.xoffset -= fol.k.xmin * 0.0152f;
                fol.k.yoffset -= fol.k.ymin * 0.0152f;
            }
            else
            {
                fol.k.xmax *= 1.01f;
                fol.k.xmin *= 1.01f;
                fol.k.ymax *= 1.01f;
                fol.k.ymin *= 1.01f;
                fol.k.xoffset += fol.k.xmin * 0.0152f;
                fol.k.yoffset += fol.k.ymin * 0.0152f;
            }
            if (*keypress & 1UL << 5)
            {
                fol.k.xmax -= fol.zoom;
                fol.k.xmin -= fol.zoom;
                fol.k.ymax -= fol.zoom;
                fol.k.ymin -= fol.zoom;
            }
            else if (*keypress & 1UL << 6)
            {
                fol.k.xmax += fol.zoom;
                fol.k.xmin += fol.zoom;
                fol.k.ymax += fol.zoom;
                fol.k.ymin += fol.zoom;
            }
            //fol.k.xoffset += (((double)0.5f * fol.k.xmax) - fol.k.xmin);
            //fol.k.yoffset += (((double)0.5f * fol.k.ymax) - fol.k.ymin);
        }
#if 1
        char TempBuffer[256];
        sprintf(TempBuffer, "\txmin = %f xmax = %f\n\tymin = %f ymax = %f\n\tzoom = %f\n\txoffset = %f yoffset = %f\n", fol.k.xmax, fol.k.xmin, fol.k.ymax, fol.k.ymin, fol.zoom, fol.k.xoffset, fol.k.yoffset);
        OutputDebugStringA(TempBuffer);
#endif
        run_cl(Buffer, &fol);
    }
    fol.zoom = 1;
    *keypress &= ~(1UL << 17);
    *keypress &= ~(1UL << 18);
}


