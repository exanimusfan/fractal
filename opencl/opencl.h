/*   opencl.h                                                                 */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2019/06/03 11:26:19 by V Caraulan                               */

#ifndef OPENCL_H
# define OPENCL_H

typedef struct	s_jul
{
	int		x_dim;
	int		y_dim;
	float	zx;
	float	zy;
	float	ci;
	float	cr;
	float	x_origin;
	float	y_origin;
	size_t	width;
	size_t	height;
}				t_jul;

typedef struct	s_krn
{
	float	xmin;
	float	xmax;
	float	ymin;
	float	ymax;
	float	xoffset;
	float	yoffset;
	int		iter;
	float	red;
	float	green;
	float	blue;
}				t_krn;

typedef struct	s_mdl
{
	float	x;
	float	y;
	int		x_dim;
	int		y_dim;
	float	x_origin;
	float	y_origin;
	size_t	width;
	size_t	height;
}				t_mdl;

#endif
