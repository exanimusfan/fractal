/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sierpinski.cl                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/17 05:59:35 by viccarau          #+#    #+#             */
/*   Updated: 2019/07/11 17:04:59 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <opencl.h>

static int		ft_color(t_krn k, t_jul j, int i)
{
	int		r;
	int		g;
	int		b;
	double	color_const;

	color_const = (i + 1 - (log(2.0) / (log(j.zx * j.zx + j.zy * j.zy)))
		/ log(2.0));
	r = sin(k.red * color_const) * 127.5 + 127.5;
	g = sin(k.green * color_const) * 127.5 + 127.5;
	b = sin(k.blue * color_const) * 127.5 + 127.5;
	return ((r << 16) | (g << 8) | b);
}

static t_jul	init_jul(int x, int y, t_krn k)
{
	t_jul	j;

	j.x_dim = get_global_id(0) + k.xoffset;
	j.y_dim = get_global_id(1) + k.yoffset;
	j.width = get_global_size(0);
	j.height = get_global_size(1);
	return (j);
}

__kernel void	render(__global int *out, t_krn k, int x, int y)
{
	t_jul	j;
	int		i;
	int		p;
	double	xtemp;
	p = 0;
	i = 1;
	j = init_jul(x, y, k);
	while (j.x_dim > 0 || j.x_dim > 0)
	{
		if (j.x_dim % 3 == 1 && j.y_dim % 3 == 1)
		{
			out[(j.width * get_global_id(1)) + get_global_id(0)] = get_global_id(1)
				+ get_global_id(0) + k.xmax;
			p = 1;
		}
		j.x_dim /= 3;
		j.y_dim /= 3;
	}
	if (!p)
	{
		out[(j.width * get_global_id(1)) + get_global_id(0)] = get_global_id(1)
			+ get_global_id(0);
	}
	else
		out[(j.width * get_global_id(1)) + get_global_id(0)] = j.y_dim + j.x_dim + k.xmax;
}