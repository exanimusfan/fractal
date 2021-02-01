/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   fractal.c                                                                */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#include "constant_strings.c"

internal int
finish_cl(cl_context context,
          cl_command_queue *cmd_queue, cl_uint num_devices, cl_mem image)
{
    cl_uint i;

    i = 0;
    if (image)
		clReleaseMemObject(image);
    while (i < num_devices)
    {
        if (cmd_queue[i] != NULL)
            clReleaseCommandQueue(cmd_queue[i]);
        i++;
    }
	if (context)
		clReleaseContext(context);
	return (0);
}

#include <windows.h>
#include <stdio.h>

internal void
check_succeeded(char *message, cl_int err)
{
    if (err != CL_SUCCESS)
    {
        char TempBuffer[256];
        sprintf_s(TempBuffer, 256, "%s %d\n", message, err);
        MessageBox(NULL, TempBuffer, TempBuffer, MB_ICONEXCLAMATION | MB_OK);
    }
}

internal void
print_debug_info(cl_context context)
{
	t_dbug			d = {0};
	size_t			size;
	size_t			elements;
	int				i;

	d.err = clGetContextInfo(context, CL_CONTEXT_DEVICES,
                             sizeof(cl_device_id) * 16, &d.devices, &size);
	check_succeeded("Getting context info", d.err);
	elements = size / sizeof(cl_device_id);
	i = 0;
	while (i < elements)
	{
		d.err = clGetDeviceInfo(d.devices[i], CL_DEVICE_VENDOR,
                                sizeof(d.vendor_name), d.vendor_name, NULL);
		d.err |= clGetDeviceInfo(d.devices[i], CL_DEVICE_NAME,
                                 sizeof(d.device_name), d.device_name, NULL);
        check_succeeded("Getting device info", d.err);
        i++;
	}
}

internal void
get_context(application_offscreen_buffer Buffer, t_fol *fol, int i, HGLRC GLContext)
{
    t_OpenCL       *o;
    cl_int         err;
    cl_device_id   devices[16] = {0};
    cl_platform_id platform = {0};

    o            = &fol->ocl;
	o->buff_size = Buffer.BytesPerPixel * Buffer.Width * Buffer.Height;
    clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 16, devices,
                         &o->num_devices);

    o->context = clCreateContext(0, o->num_devices, devices, NULL, NULL, &err);
    check_succeeded("Creating context", err);
    if (o->num_devices == 0)
	{
        MessageBox(NULL, "No compute devices found\n",
                   NULL, MB_ICONEXCLAMATION | MB_OK);
    }

    print_debug_info(o->context);
    fol->ocl.err = clGetContextInfo(o->context, CL_CONTEXT_DEVICES,
                                    sizeof(cl_device_id) * 16, &o->devices, NULL);
    check_succeeded("Getting context info", o->err);

    while ((cl_uint)i < o->num_devices)
	{
		o->cmd_queue[i] = clCreateCommandQueueWithProperties(o->context,
                                                             o->devices[i],
                                                             0, &o->err);
        check_succeeded("Creating command queue", o->err);
		i++;
	}
}

internal void
BuildErrors(cl_program program, void *user_data)
{
    cl_device_id *device;
    char Output[256];
    int err     = 0;
    size_t size = 0;

    device = user_data;
    err = clGetProgramBuildInfo (program, *device, CL_PROGRAM_BUILD_LOG,
                                 sizeof(Output), &Output[0], &size);

    if (err != 0)
        MessageBox(NULL, Output, "Build Error\n", MB_ICONEXCLAMATION | MB_OK);
}

internal cl_kernel
load_krnl(cl_device_id device, cl_context context)
{
    cl_program program[1];
    cl_kernel  kernel[1];
    char       *program_source;
    int        err = 0;
    program_source = MandelbrotClSource();
    if (program_source != NULL)
	{
        program[0] = clCreateProgramWithSource(context, 1,
                                               (const char**)&program_source,
                                               NULL, &err);
        check_succeeded("Loading kernel", err);
		err = clBuildProgram(program[0], 0, NULL,
                             "-cl-finite-math-only -cl-unsafe-math-optimizations",
                             BuildErrors, &device);
        check_succeeded("Building program", err);
		kernel[0] = clCreateKernel(program[0], "render", &err);
        check_succeeded("Create Kernel", err);
    }
    return (kernel[0]);
}

internal inline f64
ClampF(f64 value, f64 min, f64 max)
{
    f64 Result;

    Result = value;
    if (value > max)
        Result = max;
    else if (value < min)
        Result = min;
    return (Result);
}

internal inline t_fol
InitFractolStructure(void)
{
    t_fol Result = {0};

    Result.Kernel.xoffset = -3.56f;
    Result.Kernel.yoffset = -1.2f;
    Result.Kernel.iter    = 1500;
    Result.accel.x        = 0;
    Result.accel.y        = 0;
    return (Result);
}

internal inline void
RenderOnCPU(t_Kernel Kernel, application_offscreen_buffer Buffer)
{
    char *Image;
    Image = (char *)Buffer.Memory;

    for (int YDimension = 0;YDimension < Buffer.Height; YDimension ++)
    {
        for (int XDimension = 0; XDimension < Buffer.Width; XDimension++)
        {
            f64 cr    = ((f32)XDimension / Buffer.Width * Kernel.xmax
                         - Kernel.xmin) + Kernel.xoffset;
            f64 ci    = ((f32)YDimension / Buffer.Height * Kernel.ymax
                         - Kernel.ymin) + Kernel.yoffset;
            int i     = 0;
            f64 zr    = 0;
            f64 zi    = 0;
            f64 zrsqr = 0;
            f64 zisqr = 0;
            Image[(Buffer.Width * YDimension * 4) + XDimension * 4 + 0] = 0;
            Image[(Buffer.Width * YDimension * 4) + XDimension * 4 + 1] = 0;
            Image[(Buffer.Width * YDimension * 4) + XDimension * 4 + 2] = 0;
            while (i < Kernel.iter)
            {
                zi = zr * zi;
                zi += zi;
                zi += ci;
                zr = zrsqr - zisqr + cr;
                zrsqr = zr * zr;
                zisqr = zi * zi;
                if (zrsqr + zisqr > 4.0)
                {
                    float color_const = (float)(i + 1 - (log(1.5) / (log(zr * zr + zi * zi))) / log(2.0));
                    char color1 = (char)(sin(0.019 * color_const) * 127.5 + 127.5);
                    char color2 = (char)(sin(0.017 * color_const) * 127.5 + 127.5);
                    char color3 = (char)(sin(0.015 * color_const) * 127.5 + 127.5);
                    Image[(Buffer.Width * YDimension * 4) + XDimension * 4 + 0] = color1;
                    Image[(Buffer.Width * YDimension * 4) + XDimension * 4 + 1] = color2;
                    Image[(Buffer.Width * YDimension * 4) + XDimension * 4 + 2] = color3;
                    break;
                }
                i++;
            }
        }
    }
}

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer,
						   application_input_handle Input, int Render, f64 RenderPercent, HGLRC glContext)
{
    local_persist t_fol Fractol;

    if (Fractol.flag == 0)
    {
        Fractol = InitFractolStructure();
        Fractol.flag ^= FLAG_INITIALIZED;

        float xrange = 2;
        float yrange = xrange / ((float)Buffer.Width / (float)Buffer.Height);
        Fractol.x = Buffer.Width / 2;
        Fractol.y = Buffer.Height / 2;
        Fractol.Kernel.xmin = -xrange;
        Fractol.Kernel.xmax = xrange;
        Fractol.Kernel.ymin = -yrange;
        Fractol.Kernel.ymax = yrange;
    }
    if (((float)Buffer.Width / (float)Buffer.Height) !=
        (Fractol.Kernel.xmax / Fractol.Kernel.ymax))
    {
        float xrange = (float)Fractol.Kernel.xmax;
        float yrange = xrange / ((float)Buffer.Width / (float)Buffer.Height);
        Fractol.Kernel.xmin = -xrange;
        Fractol.Kernel.xmax = xrange;
        Fractol.Kernel.ymin = -yrange;
        Fractol.Kernel.ymax = yrange;
    }
    if (Input.MouseRelativePos.x || Input.MouseRelativePos.y)
    {
        Fractol.Kernel.xoffset -= ((Fractol.Kernel.xmax * RenderPercent) /
                                   (f64)Buffer.Width)
            * (f64)Input.MouseRelativePos.x;
        Fractol.Kernel.yoffset -= ((Fractol.Kernel.ymax * RenderPercent) /
                                   (f64)Buffer.Height)
            * (f64)Input.MouseRelativePos.y;
    }

    if (Input.MouseWheel != 0)
    {
        f64 xmaxBefore = Fractol.Kernel.xmax;
        f64 ymaxBefore = Fractol.Kernel.ymax;

        if (!(Fractol.Kernel.xmax < 8.0e-14 && Input.MouseWheel < 0) &&
            !(Fractol.Kernel.xmax > 20 && Input.MouseWheel > 0))
        {
            Fractol.Kernel.xmin += (Input.MouseWheel * 0.001f) * Fractol.Kernel.xmin;
            Fractol.Kernel.xmax += (Input.MouseWheel * 0.001f) * Fractol.Kernel.xmax;
            Fractol.Kernel.ymin += Fractol.Kernel.ymin * (Input.MouseWheel * 0.001f);
            Fractol.Kernel.ymax += Fractol.Kernel.ymax * (Input.MouseWheel * 0.001f);
            if (Fractol.Kernel.xmin > 0 && Fractol.Kernel.xmax < 0)
            {
                Fractol.Kernel.xmin = -xmaxBefore;
                Fractol.Kernel.xmax = xmaxBefore;
                Fractol.Kernel.ymin = -ymaxBefore;
                Fractol.Kernel.ymax = ymaxBefore;
            }
            else
            {
                Fractol.Kernel.xoffset += (xmaxBefore - Fractol.Kernel.xmax) * 1.5f;
                Fractol.Kernel.yoffset += (ymaxBefore - Fractol.Kernel.ymax) * 1.5f;
            }
}
        Input.MouseWheel = 0;

    }


    if (Fractol.accel.x < 0.00001f && Fractol.accel.x >= 0.0f)
        Fractol.accel.x = 0;
    else if (Fractol.accel.x > -0.00001f && Fractol.accel.x <= 0.0f)
        Fractol.accel.x = 0;
    if (Fractol.accel.y < 0.00001f && Fractol.accel.y > 0.0f)
        Fractol.accel.y = 0;
    else if (Fractol.accel.y > -0.00001f && Fractol.accel.y <= 0.0f)
        Fractol.accel.y = 0;
    if (Input.KeyPress & (1UL << KEY_1))
        Fractol.Kernel.iter = 3000;
    if (Input.KeyPress & (1UL << KEY_2))
        Fractol.Kernel.iter = 5000;
    if (Input.KeyPress & 1UL << KEY_PLUS)
        Fractol.Kernel.iter += 100;
    if (Input.KeyPress & 1UL << KEY_MINUS)
        Fractol.Kernel.iter -= 100;

    if (!(Fractol.flag & (1UL << FLAG_CL_INITIALIZED)))
    {
        get_context(Buffer, &Fractol, 0, glContext);
        Fractol.ocl.image = clCreateBuffer(Fractol.ocl.context, CL_MEM_WRITE_ONLY,
                                           Fractol.ocl.buff_size, NULL,
                                           &Fractol.ocl.err);
        check_succeeded("Creating buffer", Fractol.ocl.err);
        Fractol.ocl.Kernel = load_krnl(Fractol.ocl.devices[0], Fractol.ocl.context);
        Fractol.flag |= 1UL << FLAG_CL_INITIALIZED;
    }

    if (Fractol.ocl.buff_size < Buffer.BytesPerPixel * Buffer.Width * Buffer.Height)
    {
        clFinish(Fractol.ocl.cmd_queue[0]);
        finish_cl(Fractol.ocl.context,
                  Fractol.ocl.cmd_queue,
                  Fractol.ocl.num_devices,
                  Fractol.ocl.image);
        get_context(Buffer, &Fractol, 0, glContext);
        Fractol.ocl.image = clCreateBuffer(Fractol.ocl.context, CL_MEM_WRITE_ONLY,
                                           Fractol.ocl.buff_size, NULL,
                                           &Fractol.ocl.err);
        check_succeeded("Creating buffer", Fractol.ocl.err);
        Fractol.ocl.Kernel = load_krnl(Fractol.ocl.devices[0], Fractol.ocl.context);
        Fractol.ocl.buff_size = Buffer.BytesPerPixel * Buffer.Width * Buffer.Height;
    }
    if ((Render && RenderPercent > 0.5f) || (Fractol.Kernel.iter >= 10000 && Render))
    {
        Fractol.ocl.err = clSetKernelArg(Fractol.ocl.Kernel, 0,sizeof(cl_mem),
                                         &Fractol.ocl.image);
        check_succeeded("Setting kernel arg", Fractol.ocl.err);
        Fractol.ocl.err |= clSetKernelArg(Fractol.ocl.Kernel, 1, sizeof(t_Kernel),
                                          &Fractol.Kernel);
        check_succeeded("Setting kernel arg", Fractol.ocl.err);
        Fractol.ocl.err |= clSetKernelArg(Fractol.ocl.Kernel, 2, sizeof(int),
                                          &Fractol.x);
        check_succeeded("Setting kernel arg", Fractol.ocl.err);
        Fractol.ocl.err |= clSetKernelArg(Fractol.ocl.Kernel, 3, sizeof(int),
                                          &Fractol.y);
        check_succeeded("Setting kernel arg", Fractol.ocl.err);

        size_t d_size[2];
        d_size[0] = Buffer.Width;
        d_size[1] = Buffer.Height;
        if (Buffer.Width > 0 && Buffer.Height > 0){
            Fractol.ocl.err = clEnqueueNDRangeKernel(Fractol.ocl.cmd_queue[0],
                                                     Fractol.ocl.Kernel, 2, 0, d_size,
                                                     NULL, 0, NULL, NULL);
            Fractol.img = Buffer.Memory;
            check_succeeded("Running kernel", Fractol.ocl.err);
            Fractol.ocl.err = clEnqueueReadBuffer(Fractol.ocl.cmd_queue[0],
                                                  Fractol.ocl.image, CL_FALSE, 0,
                                                  Buffer.BytesPerPixel * Buffer.Width *
                                                  Buffer.Height,
                                                  Fractol.img,
                                                  0, NULL, NULL);
            check_succeeded("Reading buffer", Fractol.ocl.err);

        }
        clFinish(Fractol.ocl.cmd_queue[0]);
    }
    else if (Render && Fractol.Kernel.iter < 10000)
        RenderOnCPU(Fractol.Kernel, Buffer);
}
