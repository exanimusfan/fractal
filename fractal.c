/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   fractal.c                                                                */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/12 13:40:21 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#include "constant_strings.c"

#if 0

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
    OutputDebugStringA(&StringNumber[0]);
    OutputDebugStringA(" Test\n");
}

#endif
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

// TODO(V Caraulan): Remove this
#include <windows.h>
#include <stdio.h>
//

internal void
check_succeeded(char *message, cl_int err)
{
    if (err != CL_SUCCESS)
    {
        char TempBuffer[256];
        sprintf_s(TempBuffer, 255, "%s %d\n", message, err);
        OutputDebugStringA(TempBuffer);
        MessageBox(NULL, TempBuffer, TempBuffer, MB_ICONEXCLAMATION | MB_YESNO);
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
get_context(application_offscreen_buffer Buffer, t_fol *fol, int i)
{
    t_OpenCL       *o;
    cl_int         err;
    cl_device_id   devices[16] = {0};
    cl_platform_id platform = {0};
    
    o            = &fol->ocl;
	//o->buff_size = Buffer.BytesPerPixel * Buffer.Width * Buffer.Height;
    o->buff_size = Buffer.BytesPerPixel * 7680 * 4320;
    clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 16, devices,
                         &o->num_devices);
    
    o->context = clCreateContext(0, o->num_devices, devices, NULL, NULL, &err);
    check_succeeded("Creating context", err);
    if (o->num_devices == 0)
	{
        //OutputDebugStringA("No compute devices found\n");
        MessageBox(NULL, "No compute devices found\n",
                   NULL, MB_ICONEXCLAMATION | MB_YESNO);
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
    int err = 0;
    size_t size = 0;
    
    device = user_data;
    err = clGetProgramBuildInfo (program, *device, CL_PROGRAM_BUILD_LOG,
                                 sizeof(Output), &Output[0], &size);
    //char TempBuffer[512];
    //sprintf_s(TempBuffer, 512, "Build info :\n%sEND\nsize = %lld\n", Output, size);
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
    
    Result.Kernel.red = 0.002f;
    Result.Kernel.green = 0.003f;
    Result.Kernel.blue = 0.005f;
    Result.Kernel.xoffset = -3.56f;
    Result.Kernel.yoffset = -1.2f;
    Result.Kernel.iter = 1500;
    Result.accel.x = 0;
    Result.accel.y = 0;
    return (Result);
}

internal inline int
ft_color(t_Kernel k, t_mdl m, int i) 
{ 
    int r; int g; int b; double color_const; 
    color_const = (i + 1 - (log(2.0) / (log(m.x * m.x + m.y * m.y))) / log(2.0)); 
    r = (int)(sin(k.red * color_const) * 127.5 + 127.5); 
    g = (int)(sin(k.green * color_const) * 127.5 + 127.5); 
    b = (int)(sin(k.blue * color_const) * 127.5 + 127.5);
    return ((r << 16) | (g << 8) | b); 
}

internal void
ApplicationUpdateAndRender(application_offscreen_buffer Buffer,
						   application_input_handle Input, int Render, f64 RenderPercent)
{
    local_persist t_fol Fractol;
    
    if (Fractol.flag == 0)
    {
        Fractol = InitFractolStructure();
        Fractol.flag ^= FLAG_INITIALIZED;
        
        // TODO(V Caraulan): These values below should be recalculated everytime
        // if buffer changes ?????
        //float xrange = 6;
        float xrange = 2;
        float yrange = xrange / ((float)Buffer.Width / (float)Buffer.Height);
        Fractol.img = Buffer.Memory;
        Fractol.x = Buffer.Width / 2;
        Fractol.y = Buffer.Height / 2;
        Fractol.Kernel.xmin = -xrange;
        Fractol.Kernel.xmax = xrange;
        Fractol.Kernel.ymin = -yrange;
        Fractol.Kernel.ymax = yrange;
    }
    if (((float)Buffer.Width / (float)Buffer.Height) != (Fractol.Kernel.xmax / Fractol.Kernel.ymax))
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
        Fractol.Kernel.xoffset -= ((Fractol.Kernel.xmax * RenderPercent) / (f64)Buffer.Width)
            * (f64)Input.MouseRelativePos.x;
        Fractol.Kernel.yoffset -= ((Fractol.Kernel.ymax * RenderPercent) / (f64)Buffer.Height)
            * (f64)Input.MouseRelativePos.y;
    }
    
    // TODO(V Caraulan): Scroll depending on its value, not direction
    
    if (Input.MouseWheel != 0)
    {
        f64 xmaxBefore = Fractol.Kernel.xmax;
        f64 ymaxBefore = Fractol.Kernel.ymax;
        
        Fractol.Kernel.xmin += Fractol.Kernel.xmin * (Input.MouseWheel * 0.001f);
        Fractol.Kernel.xmax += Fractol.Kernel.xmax * (Input.MouseWheel * 0.001f);
        Fractol.Kernel.ymin += Fractol.Kernel.ymin * (Input.MouseWheel * 0.001f);
        Fractol.Kernel.ymax += Fractol.Kernel.ymax * (Input.MouseWheel * 0.001f);
        Fractol.Kernel.xoffset += (xmaxBefore - Fractol.Kernel.xmax) * 1.5f;
        Fractol.Kernel.yoffset += (ymaxBefore - Fractol.Kernel.ymax) * 1.5f;
        Input.MouseWheel = 0;
    }
    
    // TODO(V Caraulan): This offset happens when zooming in so it's in center.
    //Could change it so it zooms in depending on mouse position
    //Fractol.Kernel.xoffset += (xmaxBefore - Fractol.Kernel.xmax) * 1.5;
    //Fractol.Kernel.yoffset += (ymaxBefore - Fractol.Kernel.ymax) * 1.5;
    
    
    // TODO(V Caraulan): Any way to this in a more elegant way ?
    if (Fractol.accel.x < 0.00001f && Fractol.accel.x >= 0.0f)
        Fractol.accel.x = 0;
    else if (Fractol.accel.x > -0.00001f && Fractol.accel.x <= 0.0f)
        Fractol.accel.x = 0;
    if (Fractol.accel.y < 0.00001f && Fractol.accel.y > 0.0f)
        Fractol.accel.y = 0;
    else if (Fractol.accel.y > -0.00001f && Fractol.accel.y <= 0.0f)
        Fractol.accel.y = 0;
    //
    
    if (Input.KeyPress & (1UL << KEY_1))
        Fractol.Kernel.iter = 3000;
    if (Input.KeyPress & (1UL << KEY_2))
        Fractol.Kernel.iter = 5000;
    if (Input.KeyPress & 1UL << KEY_PLUS)
        Fractol.Kernel.iter += 100;
    if (Input.KeyPress & 1UL << KEY_MINUS)
        Fractol.Kernel.iter -= 100;
    if (Input.KeyPress & 1UL << KEY_R)
        Fractol.Kernel.red += Fractol.Kernel.red / 1000;
    if (Input.KeyPress & 1UL << KEY_T)
        Fractol.Kernel.red -= Fractol.Kernel.red / 1000;
    if (Input.KeyPress & 1UL << KEY_G)
        Fractol.Kernel.green += Fractol.Kernel.green / 1000;
    if (Input.KeyPress & 1UL << KEY_H)
        Fractol.Kernel.green -= Fractol.Kernel.green / 1000;
    if (Input.KeyPress & 1UL << KEY_B)
        Fractol.Kernel.blue += Fractol.Kernel.blue / 1000;
    if (Input.KeyPress & 1UL << KEY_N)
        Fractol.Kernel.blue -= Fractol.Kernel.blue / 1000;
    
    Fractol.Kernel.red = ClampF(Fractol.Kernel.red, 0.001f, 0.5f);
    Fractol.Kernel.green = ClampF(Fractol.Kernel.green, 0.001f, 0.5f);
    Fractol.Kernel.blue = ClampF(Fractol.Kernel.blue, 0.001f, 0.5f);
    // TODO(V Caraulan): Group this with the other change if buffer changes ?
    if (!(Fractol.flag & (1UL << FLAG_CL_INITIALIZED)))
    {
        get_context(Buffer, &Fractol, 0);
        Fractol.ocl.image = clCreateBuffer(Fractol.ocl.context, CL_MEM_WRITE_ONLY,
                                           Fractol.ocl.buff_size, NULL,
                                           &Fractol.ocl.err);
        check_succeeded("Creating buffer", Fractol.ocl.err);
        Fractol.ocl.Kernel = load_krnl(Fractol.ocl.devices[0], Fractol.ocl.context);
        Fractol.flag |= 1UL << FLAG_CL_INITIALIZED;
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
        Fractol.ocl.err = clEnqueueNDRangeKernel(Fractol.ocl.cmd_queue[0],
                                                 Fractol.ocl.Kernel, 2, 0, d_size,
                                                 NULL, 0, NULL, NULL);
        
        check_succeeded("Running kernel", Fractol.ocl.err);
        Fractol.ocl.err = clEnqueueReadBuffer(Fractol.ocl.cmd_queue[0],
                                              Fractol.ocl.image, CL_FALSE, 0,
                                              Fractol.ocl.buff_size, Fractol.img,
                                              0, NULL, NULL);
        check_succeeded("Reading buffer", Fractol.ocl.err);
        clFinish(Fractol.ocl.cmd_queue[0]);
    }// TODO(Victor Caraulan): This 10000 iter is a good ideea, but not the way it's implemented right now.
    else if (Render && Fractol.Kernel.iter < 10000)
    {
        int x = 0;
        int y = 0;
        
        while (y < Buffer.Height)
        {
            x = 0;
            while (x < Buffer.Width)
            {
                t_mdl  m; 
                f64 xtemp;
                int    i; 
                
                m.x_dim = x;
                m.y_dim = y;
                m.width = Buffer.Width;
                m.height = Buffer.Height; 
                m.x_origin = ((f64)m.x_dim / m.width * Fractol.Kernel.xmax - Fractol.Kernel.xmin) + Fractol.Kernel.xoffset; 
                m.y_origin = ((f64)m.y_dim / m.height * Fractol.Kernel.ymax - Fractol.Kernel.ymin) + Fractol.Kernel.yoffset; 
                m.x = 0.0; 
                m.y = 0.0; 
                i = 0; 
                xtemp = m.x * m.x - m.y * m.y + m.x_origin; 
                while (m.x * m.x + m.y * m.y <= 4 && i < Fractol.Kernel.iter) 
                { 
                    xtemp = m.x * m.x - m.y * m.y + m.x_origin; 
                    m.y = 2 * m.x * m.y + m.y_origin; 
                    m.x = xtemp; 
                    i++; 
                } 
                if (i == Fractol.Kernel.iter) 
                    Fractol.img[(m.width * m.y_dim) + m.x_dim] = 0; 
                else 
                    Fractol.img[(m.width * m.y_dim) + m.x_dim] = ft_color(Fractol.Kernel, m, i);
                x++;
            }
            y++;
        }
    }
}
