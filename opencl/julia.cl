/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   julia.cl                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/02 12:39:32 by viccarau          #+#    #+#             */
/*   Updated: 2019/06/17 01:33:25 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "opencl.h"

static int		ft_color(t_krn k, t_jul j, int i)
{
	int r;
	int g;
	int b;
	double color_const;

	color_const = (i + 1 - (log(2.0) / (log(j.zx * j.zx + j.zy * j.zy)))
		/ log(2.0));
	r = sin(k.red * color_const) * 127.5 + 127.5;
	g = sin(k.green * color_const) * 127.5 + 127.5;
	b = sin(k.blue * color_const) * 127.5 + 127.5;
	return ((r << 16) | (g << 8) | b);
}

static t_jul	init_jul(int x, int y, t_krn k)
{
	t_jul j;

	j.x_dim = get_global_id(0);
	j.y_dim = get_global_id(1);
	j.width = get_global_size(0);
	j.height = get_global_size(1);
	if (x == j.width / 2 && y == j.height / 2)
	{
		j.ci = -0.034375f;
		j.cr = 0.336250f;
	}
	else
	{
		j.ci = ((double)y / j.height * 2.5f - 1.25f);
		j.cr = ((double)x / j.width * 3.5f - 2.0f);
	}
	j.zx = ((((double)j.x_dim / j.width) * k.xmax - k.xmin)) + k.xoffset;
	j.zy = ((((double)j.y_dim / j.height) * k.ymax - k.ymin)) + k.yoffset;
	return (j);
}

__kernel void	render(__global int *out, t_krn k, int x, int y)
{
	t_jul	j;
	int		i;
	double	xtemp;

	j = init_jul(x, y, k);
	i = 0;
	while (j.zx * j.zx + j.zy * j.zy <= 4 && i < k.iter)
	{
		xtemp = j.zx * j.zx - j.zy * j.zy;
		j.zy = 2 * j.zx * j.zy + j.ci;
		j.zx = xtemp + j.cr;
		i++;
	}
	if (i == k.iter)
		out[(j.width * j.y_dim) + j.x_dim] = 0;
	else
		out[(j.width * j.y_dim) + j.x_dim] = ft_color(k, j, i);
}
