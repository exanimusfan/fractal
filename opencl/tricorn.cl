/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tricorn.cl                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 14:22:14 by viccarau          #+#    #+#             */
/*   Updated: 2019/06/16 22:58:42 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "opencl.h"

static int		ft_color(t_krn k, t_mdl m, int i)
{
	int r;
	int g;
	int b;
	double color_const;

	color_const = (i + 1 - (log(2.0) / (log(m.x * m.x + m.y * m.y)))
				 / log(2.0));
	r = sin(k.red * color_const) * 127.5 + 127.5;
	g = sin(k.green * color_const) * 127.5 + 127.5;
	b = sin(k.blue * color_const) * 127.5 + 127.5;
	return ((r << 16) | (g << 8) | b);
}

static t_mdl	mdl_init(t_krn k, double xoffset, double yoffset)
{
	t_mdl	m;

	m.x_dim = get_global_id(0);
	m.y_dim = get_global_id(1);
	m.width = get_global_size(0);
	m.height = get_global_size(1);
	m.x_origin = ((((double)m.x_dim / m.width) * k.xmax - k.xmin)) + k.xoffset;
	m.y_origin = ((((double)m.y_dim / m.height) * k.ymax - k.ymin)) + k.yoffset;
	m.x = 0.0;
	m.y = 0.0;
	return (m);
}

__kernel void	render(__global int *out, t_krn k, int mx, int my)
{
	t_mdl	m;
	double	xtemp;
	int		i;

	m = mdl_init(k, k.xoffset, k.yoffset);
	i = 0;
	xtemp = m.x * m.x - m.y * m.y + m.x_origin;
	while (m.x * m.x + m.y * m.y <= 4 && i < k.iter)
	{
		xtemp = m.x * m.x - m.y * m.y + m.x_origin;
		m.y = -2 * m.x * m.y + m.y_origin;
		m.x = xtemp;
		i++;
	}
	if (i == k.iter)
		out[(m.width * m.y_dim) + m.x_dim] = 0;
	else
		out[(m.width * m.y_dim) + m.x_dim] = ft_color(k, m, i);
}
