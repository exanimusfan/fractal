/*   opencl.h                                                                 */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2019/06/03 11:26:19 by V Caraulan                               */

#ifndef OPENCL_H
# define OPENCL_H

typedef struct	s_jul
{
	int		x_dim;
	int		y_dim;
	double	zx;
	double	zy;
	double	ci;
	double	cr;
	double	x_origin;
	double	y_origin;
	size_t	width;
	size_t	height;
}				t_jul;

typedef struct	s_krn
{
	double	xmin;
	double	xmax;
	double	ymin;
	double	ymax;
	double	xoffset;
	double	yoffset;
	int		iter;
	double	red;
	double	green;
	double	blue;
}				t_krn;

typedef struct	s_mdl
{
	double	x;
	double	y;
	int		x_dim;
	int		y_dim;
	double	x_origin;
	double	y_origin;
	size_t	width;
	size_t	height;
}				t_mdl;

#endif
