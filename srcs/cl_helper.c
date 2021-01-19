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

cl_context	create_context(cl_device_type dtype, cl_uint *num_devices)
{
	cl_int			err;
	cl_device_id	devices[16] = {0};
	cl_context		context;
	cl_platform_id	platform = {0};

    *num_devices = 0;
    clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 16, devices, num_devices);
    context = clCreateContext(0, *num_devices, devices, NULL, NULL, &err);
	check_succeeded("Creating context", err);
    return (context);
}

void		print_debug_info(cl_context context)
{
	t_dbug			d = {0};
	size_t			size;
	int				elements;
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
        DebugOut("Device: %d %s %s\n", i, d.vendor_name, d.device_name);
        i++;
	}
}

void		check_succeeded(char *message, cl_int err)
{
	if (err != CL_SUCCESS)
	{
		DebugOut("%s: %d\n", message, err);	
		exit(0);
	}
}
