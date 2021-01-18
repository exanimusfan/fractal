/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/14 06:58:18 by viccarau          #+#    #+#             */
/*   Updated: 2019/07/11 18:18:59 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

t_args				init_kernel_args(void)
{
	t_args		arg;

	arg.args[0] = "opencl/julia.cl";
	arg.args[1] = "opencl/mandelbrot.cl";
	arg.args[2] = "opencl/burn_ship.cl";
	arg.args[3] = "opencl/tricorn.cl";
	arg.args[4] = "opencl/menace.cl";
	arg.args[5] = "opencl/burn_ship2.cl";
	arg.args[6] = "opencl/sierpinski.cl";
	arg.args[7] = "opencl/mandelbrot3.cl";
	arg.args[8] = "opencl/mandelbrot5.cl";
	return (arg);
}

t_args				init_args(void)
{
	t_args a;

	a.args[0] = "julia";
	a.args[1] = "mandelbrot";
	a.args[2] = "burning ship";
	a.args[3] = "tricorn";
	a.args[4] = "menace";
	a.args[5] = "burning ship 2";
	a.args[6] = "sierpinski";
	a.args[7] = "mandelbrot2";
	a.args[8] = "mandelbrot4";
	a.args[9] = "j";
	a.args[10] = "m";
	a.args[11] = "b";
	a.args[12] = "t";
	a.args[13] = "ms";
	a.args[14] = "b2";
	a.args[15] = "s";
	a.args[16] = "m2";
	a.args[17] = "m4";
	return (a);
}

int					ft_exit(t_fol *f)
{
	finish_cl(f->ocl.context,
              f->ocl.cmd_queue[0], f->ocl.image);
	exit(0);
	return (1);
}

int					is_alloc(void *memory, const char *str)
{
	if (memory == NULL)
	{
		//ft_putstr("Couldn't allocate memory for ");
		//ft_putendl(str);
		exit(0);
	}
	return (0);
}
