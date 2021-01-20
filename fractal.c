/*   fractal.c                                                                */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */

#include "fractol.h"
#include "srcs/exit.c"
#include "srcs/cl_init.c"
#include "srcs/cl_helper.c"
#include "srcs/keyspress.c"
#include "constant_strings.c"

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
BuildErrors(cl_program program, void *user_data)
{
    cl_device_id *device;
    char Output[256];
    int err = 0;
    size_t size = 0;

    device = user_data;
    err = clGetProgramBuildInfo (program, *device,
                                 CL_PROGRAM_BUILD_LOG, sizeof(Output), &Output[0], &size);
    char TempBuffer[512];
    sprintf(TempBuffer, "Build info :\n%sEND\nsize = %lld\n", Output, size);
    OutputDebugStringA(TempBuffer);
}

internal cl_kernel
load_krnl(cl_device_id device, cl_context context)
{
    size_t	size;
    cl_program	program[1];
    cl_kernel	kernel[1];
    int		err = 0;
    char	*program_source;

    program_source = MandelbrotClSource();
    if (program_source != NULL)
	{
        program[0] = clCreateProgramWithSource(context, 1,
                                               (const char**)&program_source, NULL, &err);
        check_succeeded("Loading kernel", err);
		err = clBuildProgram(program[0], 0, NULL, "-cl-single-precision-constant -cl-finite-math-only -cl-unsafe-math-optimizations ",
                             BuildErrors, &device);
        check_succeeded("Building program", err);
		kernel[0] = clCreateKernel(program[0], "render", &err);
        check_succeeded("Create Kernel", err);
    }
    else
    {
        *(char *)(0) = 1; //NOTE: CRASH HARD
        // TODO(V Caraulan): Maybe do something else than crash hard after debug??
    }
	return (kernel[0]);
}

internal int 
run_cl(application_offscreen_buffer Buffer, t_fol *fol)
{
	unsigned int	j;

	j = 0;
    if (!(fol->flag & (1UL << FLAG_CL_INITIALIZED)))
    {
        fol->x = 0;
        fol->y = 0;
        get_context(Buffer, fol, 0);
        fol->ocl.image = clCreateBuffer(fol->ocl.context, CL_MEM_WRITE_ONLY,
                                        fol->ocl.buff_size, NULL, &fol->ocl.err);
        check_succeeded("Creating buffer", fol->ocl.err);
        fol->ocl.krnl = load_krnl(fol->ocl.devices[0], fol->ocl.context);
        fol->flag |= 1UL << FLAG_CL_INITIALIZED;
    }
    clFinish(fol->ocl.cmd_queue[0]);
    ft_init_args(fol);
    if (clBuildProgram)
        run_kernel(Buffer, fol);
    return (0);
}

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer, uint64 *keypress)
{
    local_persist t_fol fol;

    if (Buffer.Memory)
        fol.img = Buffer.Memory;
    fol.keypress = *keypress;
    if (fol.flag == 0)
    {
        float xrange;
        float yrange;

        fol.flag ^= FLAG_INITIALIZED;
        xrange = 6;
        yrange = xrange / ((float)Buffer.Width / (float)Buffer.Height);
        fol.k.red = 0.002f;
        fol.k.green = 0.003f;
        fol.k.blue = 0.005f;
        fol.k.xoffset = -9.04f;
        fol.k.yoffset = -4.84f;
        fol.k.xmin = -xrange;
        fol.k.xmax = xrange;
        fol.k.ymin = -yrange;
        fol.k.ymax = yrange;

        fol.k.iter = 1000;
        fol.x = Buffer.Width / 2;
        fol.y = Buffer.Height / 2;
        fol.keypress = 0;
        fol.accel.x = 0;
        fol.accel.y = 0;
        fol.old_buffer_size.x = Buffer.Width;
        fol.old_buffer_size.y = Buffer.Height;
        run_cl(Buffer, &fol);
    }

    if ((fol.keypress != 0 && !(fol.keypress & (1UL << RESOLUTION_LOW))) || fol.accel.x != 0 || fol.accel.y != 0)
    {
        check_keypress(&fol);
        fol.flag |= (1UL << FLAG_RESOLUTION_LOW);
        if (*keypress & (1UL << MOUSE_SCROLL_DOWN))
        {
            float xmaxBefore;
            float ymaxBefore;

            xmaxBefore = fol.k.xmax;
            ymaxBefore = fol.k.ymax;
            if (*keypress & (1UL << MOUSE_SCROLL_UP))
            {
                fol.k.xmax += fol.k.xmax * 0.1f;
                fol.k.xmin += fol.k.xmin * 0.1f;
                fol.k.ymax += fol.k.ymax * 0.1f;
                fol.k.ymin += fol.k.ymin * 0.1f;
            }
            else
            {
                fol.k.xmax -= fol.k.xmax * 0.1f;
                fol.k.xmin -= fol.k.xmin * 0.1f;
                fol.k.ymax -= fol.k.ymax * 0.1f;
                fol.k.ymin -= fol.k.ymin * 0.1f;
            }
            fol.k.xoffset += (xmaxBefore - fol.k.xmax) * 1.5;
            fol.k.yoffset += (ymaxBefore - fol.k.ymax) * 1.5;
        }
        run_cl(Buffer, &fol);
        *keypress |= (1UL << RESOLUTION_LOW);
    }
    else
    {
        if (fol.flag & (1UL << FLAG_RESOLUTION_LOW))
        {
            fol.flag &= ~(1UL << FLAG_RESOLUTION_LOW);
            run_cl(Buffer, &fol);
            *keypress &= ~(1UL << RESOLUTION_LOW);
        }
    }
    // TODO(V Caraulan): Deal with this somehow
    // (everytime I scroll I render the low and high resolution because of this reset)
    *keypress &= ~(1UL << MOUSE_SCROLL_DOWN);
    *keypress &= ~(1UL << MOUSE_SCROLL_UP);
}


