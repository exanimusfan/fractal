/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   constant_strings.c                                                       */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/19 17:31:22 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

internal char *
MandelbrotClSource()
{
    char *Source = "typedef struct	s_krn \n\
{ \n\
double	xmin; \n\
double	xmax; \n\
double	ymin; \n\
double	ymax; \n\
double	xoffset; \n\
double	yoffset; \n\
int		iter; \n\
}	t_krn; \n\
\n\
typedef struct	s_mdl \n\
{ \n\
double	x; \n\
double	y; \n\
double	x_origin; \n\
double	y_origin; \n\
unsigned int	width; \n\
unsigned int	height; \n\
int		x_dim; \n\
int		y_dim; \n\
}	t_mdl; \n\
\n\
\n\
static int		ft_color(t_krn k, t_mdl m, int i) \n\
{ \n\
int r; int g; int b; double color_const; \n\
color_const = (i + 1 - (log(2.0) / (log(m.x * m.x + m.y * m.y))) / log(2.0)); \n\
b = sin(0.015 * color_const) * 127.5 + 127.5; \n\
g = sin(0.017 * color_const) * 127.5 + 127.5; \n\
r = sin(0.019 * color_const) * 127.5 + 127.5; \n\
return ((b << 16) | (g << 8) | r); \n\
} \n\
\n\
static t_mdl	mdl_init(t_krn k, double xoffset, double yoffset) \n\
{ \n\
t_mdl	m; \n\
\n\
m.x_dim = get_global_id(0); \n\
m.y_dim = get_global_id(1); \n\
m.width = get_global_size(0); \n\
m.height = get_global_size(1); \n\
m.x_origin = ((double)m.x_dim / m.width * k.xmax - k.xmin) + k.xoffset; \n\
m.y_origin = ((double)m.y_dim / m.height * k.ymax - k.ymin) + k.yoffset; \n\
m.x = 0.0; \n\
m.y = 0.0; \n\
return (m); \n\
} \n\
\n\
__kernel void	render(__global int *out, t_krn k, int mx, int my) \n\
{ \n\
t_mdl  m; \n\
double xtemp; \n\
int i; \n\
\n\
m = mdl_init(k, k.xoffset, k.yoffset); \n\
i = 0; \n\
xtemp = m.x * m.x - m.y * m.y + m.x_origin; \n\
out[(m.width * m.y_dim) + m.x_dim] = 0; \n\
while (i < k.iter) \n\
{ \n\
xtemp = m.x * m.x - m.y * m.y + m.x_origin; \n\
m.y = 2 * m.x * m.y + m.y_origin; \n\
m.x = xtemp; \n\
if (m.x * m.x + m.y * m.y > 4) \n\
{ \n\
out[(m.width * m.y_dim) + m.x_dim] = ft_color(k, m, i); \n\
break; \n\
}\n\
i++; \n\
}}\n";
    return (Source);
}
