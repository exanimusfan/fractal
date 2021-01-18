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
    size_t size;
    char		source[4096];
	cl_program	program[1];
	cl_kernel	kernel[1];
	int		err = 0;
	char		*program_source;

    program_source = load_program_source(filename, &source[0]);
    if (program_source != NULL)
	{
        program[0] = clCreateProgramWithSource(context, 1,
                                               (const char**)&program_source, NULL, &err);
        check_succeeded("Loading kernel", err);
		err = clBuildProgram(program[0], 0, NULL, "-I opencl", NULL, NULL);
        err = clGetProgramBuildInfo (program[0], device, CL_PROGRAM_BUILD_LOG,
                                     4096, &source[0],&size);
        source[size] = '\0';
        DebugOut("Test = %s\n", source);
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
	return (0);
}

// TODO(V Caraulan): I should send mouse, keyboard and maybe controller input. Maybe under the form of a flag system.
//                    But it should have an enum for better readability

float lerp(float a, float b, float f) 
{
    return (a * (1.0 - f)) + (b * f);
}

#if 0
f32	lerp(f32 a, f32 t, f32 b)
{
	int32	result;

	result = (1.0f - t) * a + (t * b);
    char TempBuffer[256];
    sprintf(TempBuffer, "result %f\n", result);
    OutputDebugStringA(TempBuffer);
	return (result);
}
#endif

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer, int *keypress)
{
    //t_fol temp;
    local_persist t_fol fol;
    //int different = 0;

    //RenderWierdGradient(Buffer, 0, 0);
    if (fol.flag == 0 && Buffer.Memory)
    {
        fol.img = 0;
        fol.flag ^= 1UL;
        fol.flag ^= 1UL << 2;
        fol.k.red = 0.002f;
        fol.k.green = 0.003f;
        fol.k.blue = 0.005f;
        fol.zoom = 1;
#if 1
        fol.k.xmin = 2.5f;
        fol.k.xmax = 3.5f;
        fol.k.ymin = 1.25f;
        fol.k.ymax = 2.5f;
#endif
        fol.k.iter = 100;
        fol.x = Buffer.Width / 2;
        fol.y = Buffer.Height / 2;
        fol.k.xoffset = 0.74f;
        fol.k.yoffset = 0;
        fol.keypress = 0;
        fol.accel.x = 0;
        fol.accel.y = 0;
        fol.old_buffer_size.x = Buffer.Width;
        fol.old_buffer_size.y = Buffer.Height;
    }
    //temp = fol;
    fol.keypress = *keypress;
    if (Buffer.Memory)
        fol.img = Buffer.Memory;
    check_keypress(&fol);
    fol.zoom = 1;
    if (*keypress & (1UL << 17))
    {
        if (*keypress & (1UL << 18))
            fol.zoom *= 0.99f;
        else
            fol.zoom *= 1.01f;
    }
    fol.k.xmax *= fol.zoom;
    fol.k.xmin *= fol.zoom;
    fol.k.ymax *= fol.zoom;
    fol.k.ymin *= fol.zoom;
    // TODO(V Caraulan): What this if else was ment to do is to render the
    // image at a lower resolution when there's input from the user, else it should render the image at full resolution
    if (fol.keypress != 0)
    {
        run_cl(Buffer, &fol);
        char TempBuffer[256];
        sprintf(TempBuffer, "x - lerp - %f min %f max %f\ny - lerp - %f min %f max %f\n", lerp(fol.k.xmin, 0.5f, fol.k.xmax), fol.k.xmin, fol.k.xmax, lerp(fol.k.ymin, 0.5f, fol.k.ymax), fol.k.ymin, fol.k.ymax);
        OutputDebugStringA(TempBuffer);
    }
    //else
    //run_cl(Buffer, &fol);
    *keypress &= ~(1UL << 17);
    *keypress &= ~(1UL << 18);
}
