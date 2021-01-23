/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   fractal.c                                                                */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#include "constant_strings.c"


#if 0
internal int
finish_cl(cl_context context,
          cl_command_queue cmd_queue, cl_mem image)
{
	if (image != NULL)
		clReleaseMemObject(image);
	if (cmd_queue != NULL)
		clReleaseCommandQueue(cmd_queue);
	if (context != NULL)
		clReleaseContext(context);
	return (0);
}
#endif

internal void
NumberDebugOutput(int Number)
{
    char StringNumber[16];
    int Index;

    Index = 0;
    if (Number < 0)
    {
        StringNumber[0] = '-';
        Index++;
        Number = -Number;
    }
    while (Number > 10)
    {
        StringNumber[Index++] = '0' + (Number % 10);
        Number /= 10;
    }
}

internal void
check_succeeded(char *message, cl_int err)
{
    if (err != CL_SUCCESS)
	{
        *(char *)(0) = 1; //NOTE: CRASH HARD TODO
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
        //sprintf(TempBuffer, "Device: %d %s %s\n", i, d.vendor_name, d.device_name);

        i++;
	}
}

internal void
get_context(application_offscreen_buffer Buffer, t_fol *fol, int i)
{
    t_ocl          *o;
    cl_int         err;
    cl_device_id   devices[16] = {0};
    cl_platform_id platform = {0};

    o            = &fol->ocl;
	o->buff_size = Buffer.BytesPerPixel * Buffer.Width * Buffer.Height;
    clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 16, devices, &o->num_devices);

    o->context = clCreateContext(0, o->num_devices, devices, NULL, NULL, &err);
    check_succeeded("Creating context", err);
    if (o->num_devices == 0)
	{
        //OutputDebugStringA("No compute devices found\n");
        *(char *)(0) = 1; //NOTE: CRASH HARD TODO
    }

    print_debug_info(o->context);

	fol->ocl.err = clGetContextInfo(o->context, CL_CONTEXT_DEVICES,
                                    sizeof(cl_device_id) * 16, &o->devices, NULL);
    check_succeeded("Getting context info", o->err);

    while ((cl_uint)i < o->num_devices)
	{
		o->cmd_queue[i] = clCreateCommandQueueWithProperties(o->context, o->devices[i],
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
    int err = 0;
    size_t size = 0;

    device = user_data;
    err = clGetProgramBuildInfo (program, *device, CL_PROGRAM_BUILD_LOG,
                                 sizeof(Output), &Output[0], &size);
    //char TempBuffer[512];
    //sprintf(TempBuffer, "Build info :\n%sEND\nsize = %lld\n", Output, size);
    //OutputDebugStringA(TempBuffer);
}

internal cl_kernel
load_krnl(cl_device_id device, cl_context context)
{
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
		err = clBuildProgram(program[0], 0, NULL,
                             "-cl-finite-math-only -cl-unsafe-math-optimizations",
                             BuildErrors, &device);
        check_succeeded("Building program", err);
		kernel[0] = clCreateKernel(program[0], "render", &err);
        check_succeeded("Create Kernel", err);
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
    fol->ocl.err = clSetKernelArg(fol->ocl.krnl, 0, sizeof(cl_mem), &fol->ocl.image);
    fol->ocl.err |= clSetKernelArg(fol->ocl.krnl, 1, sizeof(t_krn), &fol->k);
    fol->ocl.err |= clSetKernelArg(fol->ocl.krnl, 2, sizeof(int), &fol->x);
    fol->ocl.err |= clSetKernelArg(fol->ocl.krnl, 3, sizeof(int), &fol->y);
    check_succeeded("Setting kernel arg", fol->ocl.err);

    if (clBuildProgram)
    {
        t_ocl  *o  = &fol->ocl;;
        size_t d_size[2];

        d_size[0] = Buffer.Width;
        d_size[1] = Buffer.Height;
        o->err = clEnqueueNDRangeKernel(o->cmd_queue[0], o->krnl, 2, 0,
                                        d_size, NULL, 0, NULL, NULL);

        check_succeeded("Running kernel", o->err);
        o->err = clEnqueueReadBuffer(o->cmd_queue[0], o->image, CL_FALSE,
                                     0, o->buff_size, fol->img, 0, NULL, NULL);
        check_succeeded("Reading buffer", fol->ocl.err);
    }

    return (0);
}

internal double ClampF(double value, double min, double max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return (value);
}

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer,
						   application_input_handle Input)
{
    local_persist t_fol fol;

    if (Buffer.Memory)
        fol.img = Buffer.Memory;
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
        fol.accel.x = 0;
        fol.accel.y = 0;
    }
    //     if (not_rendered)
    int	neg = 0;

    if (Input.KeyPress & 1UL << KEY_W || Input.KeyPress & 1UL << KEY_S)
	{
		neg = (Input.KeyPress & 1UL << 16) ? -1 : 1;
		fol.accel.y += neg;
        fol.k.yoffset += ((fol.k.ymax) * neg * 0.005f);
    }
	if (Input.KeyPress & 1UL << KEY_A || Input.KeyPress & 1UL << KEY_D)
	{
		neg = (Input.KeyPress & 1UL << 14) ? -1 : 1;
		fol.accel.x += neg;
        fol.k.xoffset += ((fol.k.xmax) * neg * 0.005f);
    }
	fol.accel.x -= fol.accel.x / 2;
    fol.accel.y -= fol.accel.y / 2;

    // TODO(V Caraulan): Scroll depending on its value, not direction

    // TODO(V Caraulan): This offset happens when zooming in so it's in center.
    //Could change it so it zooms in depending on mouse position
    //fol.k.xoffset += (xmaxBefore - fol.k.xmax) * 1.5;
    //fol.k.yoffset += (ymaxBefore - fol.k.ymax) * 1.5;

    // TODO(V Caraulan): Any way to this in a more elegant way ?
    if (fol.accel.x < 0.00001f && fol.accel.x >= 0.0f)
        fol.accel.x = 0;
    else if (fol.accel.x > -0.00001f && fol.accel.x <= 0.0f)
        fol.accel.x = 0;
    if (fol.accel.y < 0.00001f && fol.accel.y > 0.0f)
        fol.accel.y = 0;
    else if (fol.accel.y > -0.00001f && fol.accel.y <= 0.0f)
        fol.accel.y = 0;
    //
    if (Input.KeyPress & (1UL << KEY_1))
		fol.k.iter = 1000;
	if (Input.KeyPress & (1UL << KEY_2))
		fol.k.iter = 10000;
	if (Input.KeyPress & 1UL << KEY_PLUS)
		fol.k.iter += 100;
	if (Input.KeyPress & 1UL << KEY_MINUS)
        fol.k.iter -= 100;
	if (Input.KeyPress & 1UL << KEY_R)
		fol.k.red += fol.k.red / 1000;
	if (Input.KeyPress & 1UL << KEY_T)
		fol.k.red -= fol.k.red / 1000;
	if (Input.KeyPress & 1UL << KEY_G)
		fol.k.green += fol.k.green / 1000;
	if (Input.KeyPress & 1UL << KEY_H)
		fol.k.green -= fol.k.green / 1000;
	if (Input.KeyPress & 1UL << KEY_B)
		fol.k.blue += fol.k.blue / 1000;
	if (Input.KeyPress & 1UL << KEY_N)
		fol.k.blue -= fol.k.blue / 1000;

    fol.k.red = ClampF(fol.k.red, 0.001f, 0.5f);
    fol.k.green = ClampF(fol.k.green, 0.001f, 0.5f);
    fol.k.blue = ClampF(fol.k.blue, 0.001f, 0.5f);

    run_cl(Buffer, &fol);

}


