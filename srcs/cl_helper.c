/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_helper.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/25 01:34:26 by viccarau          #+#    #+#             */
/*   Updated: 2019/06/22 18:10:54 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

void LpoverlappedCompletionRoutine(
                                   DWORD dwErrorCode,
                                   DWORD dwNumberOfBytesTransfered,
                                   LPOVERLAPPED lpOverlapped
                                   )
{
    printf("Error code = %d, %d", dwErrorCode, dwNumberOfBytesTransfered);
}

char *load_program_source(const char *filename, char *source);

cl_kernel	load_krnl(cl_context context,
					const char *filename)
{
	char		source[4096];
	cl_program	program[1];
	cl_kernel	kernel[1];
	int			err;
	char		*program_source;

	program_source = load_program_source(filename, &source[0]);
	if (program_source != NULL)
	{
		program[0] = clCreateProgramWithSource(context, 1,
                                               (const char**)&program_source, NULL, &err);
		check_succeeded("Loading kernel", err);
		err = clBuildProgram(program[0], 0, NULL, "-I opencl", NULL, NULL);
		check_succeeded("Building program", err);
		kernel[0] = clCreateKernel(program[0], "render", &err);
	}
	return (kernel[0]);
}

cl_context	create_context(cl_device_type dtype, cl_uint *num_devices)
{
	cl_int			err;
	cl_device_id	devices[16];
	cl_context		context;
	cl_platform_id	platforms;

	*num_devices = 0;
	clGetPlatformIDs(1, &platforms, NULL);
	err = clGetDeviceIDs(platforms, dtype, 16, devices,
                         num_devices);
	context = clCreateContext(0, *num_devices, devices, NULL, NULL, &err);
	check_succeeded("Creating context", err);
	return (context);
}

void		print_debug_info(cl_context context)
{
	t_dbug			d;
	size_t			size;
	int				elements;
	int				i;

	ft_bzero(&d, sizeof(t_dbug));
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
		print_stuff((const char *)d.vendor_name,
                    (const char *)d.device_name, i);
		i++;
	}
}

void		check_succeeded(char *message, cl_int err)
{
	if (err != CL_SUCCESS)
	{
		ft_putstr(message);
		ft_putstr(": ");
		ft_putnbr(err);
		ft_putchar('\n');
		exit(0);
	}
}
