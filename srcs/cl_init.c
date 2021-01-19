/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 15:48:34 by viccarau          #+#    #+#             */
/*   Updated: 2019/10/23 02:38:37 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

int			finish_cl(cl_context context,
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

static void	ft_init_args(t_fol *fol)
{
	fol->flag |= (1UL << 4); //NOTE: The arguments have been set ???
    // TODO(V Caraulan): The enums are a must for this flag
	fol->ocl.err = clSetKernelArg(fol->ocl.krnl, 0, sizeof(cl_mem), &fol->ocl.image);
    fol->ocl.err |= clSetKernelArg(fol->ocl.krnl, 1, sizeof(t_krn), &fol->k);
    fol->ocl.err |= clSetKernelArg(fol->ocl.krnl, 2, sizeof(int), &fol->x);
    fol->ocl.err |= clSetKernelArg(fol->ocl.krnl, 3, sizeof(int), &fol->y);
    check_succeeded("Setting kernel arg", fol->ocl.err);
}

static void	get_context(application_offscreen_buffer Buffer, t_fol *fol, int i)
{
    int Index;
    char *mem;
	t_ocl *o;

    mem = (char *)&fol->ocl;
    Index = 0;
    while (Index < sizeof(t_ocl))
    {
        mem = 0;
        Index++;
    }
    //ft_bzero(&fol->ocl, sizeof(t_ocl));
    o = &fol->ocl;
	o->buff_size = Buffer.BytesPerPixel * Buffer.Width * Buffer.Height;
    o->context = create_context(fol->dtype, &o->num_devices);
    if (o->num_devices == 0)
	{
        DebugOut("No compute devices found\n");
		exit(-1);
	}
	print_debug_info(o->context);
	fol->ocl.err = clGetContextInfo(o->context, CL_CONTEXT_DEVICES,
                                    sizeof(cl_device_id) * 16, &o->devices, NULL);
    check_succeeded("Getting context info", o->err);
	while ((cl_uint)i < o->num_devices)
	{
		o->cmd_queue[i] = clCreateCommandQueueWithProperties(o->context, o->devices[i], 0, &o->err);
        check_succeeded("Creating command queue", o->err);
		i++;
	}
}

static void	run_kernel(application_offscreen_buffer Buffer, t_fol *fol)
{
	t_ocl			*o;
	size_t			d_size[2];
	size_t			d_offset[2];
	size_t			offset;
	unsigned int	i;

	i = 0;
	d_size[0] = Buffer.Width;
	d_size[1] = Buffer.Height;
	d_offset[0] = 0;
	d_offset[1] = d_size[1] * i;
	offset = d_offset[1] * 3 * Buffer.Width;
	o = &fol->ocl;
	while (i < fol->ocl.num_devices)
	{
		o->err = clEnqueueNDRangeKernel(o->cmd_queue[i], o->krnl, 2,
                                        d_offset, d_size, NULL, 0, NULL, NULL);
		check_succeeded("Running kernel", o->err);
        o->err = clEnqueueReadBuffer(o->cmd_queue[i], o->image, CL_FALSE, offset, (o->buff_size / o->num_devices), fol->img, 0, NULL, NULL);
        check_succeeded("Reading buffer", fol->ocl.err);
        i++;
	}
}
