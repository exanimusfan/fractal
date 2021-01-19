/*   constant_strings.c                                                       */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/19 17:31:22 by V Caraulan                               */

internal char *
MandelbrotClSource()
{
    char *Source = "#include \"opencl.h\" \n\
                                                                    \
                                                                    static int		ft_color(t_krn k, t_mdl m, int i) \n\
                                                                { \n\
                                                                    int r; int g; int b; float color_const; \n\
                                                                    color_const = (i + 1 - (log(2.0) / (log(m.x * m.x + m.y * m.y))) \n\
                                                                                   / log(2.0)); \n\
                                                                    r = sin(k.red * color_const) * 127.5 + 127.5; \n\
                                                                    g = sin(k.green * color_const) * 127.5 + 127.5; \n\
                                                                    b = sin(k.blue * color_const) * 127.5 + 127.5; \n\
                                                                    return ((r << 16) | (g << 8) | b); \n\
                                                                } \n\
                                                            \n\
                                                                    static t_mdl	mdl_init(t_krn k, float xoffset, float yoffset) \n\
                                                                { \n\
                                                                    t_mdl	m; \n\
                                                        \n\
                                                        m.x_dim = get_global_id(0); \n\
                                                                m.y_dim = get_global_id(1); \n\
                                                                m.width = get_global_size(0); \n\
                                                                m.height = get_global_size(1); \n\
                                                                m.x_origin = ((float)m.x_dim / m.width * k.xmax - k.xmin) + k.xoffset; \n\
                                                                m.y_origin = ((float)m.y_dim / m.height * k.ymax - k.ymin) + k.yoffset; \n\
                                                                m.x = 0.0; \n\
                                                                m.y = 0.0; \n\
                                                                return (m); \n\
                                                            } \n\
                                                         \n\
                                                            __kernel void	render(__global int *out, t_krn k, int mx, int my) \n\
                                                            { \n\
                                                                t_mdl	m; \n\
                                                                float	xtemp; \n\
                                                                int		i; \n\
                                                        \n\
                                                                m = mdl_init(k, k.xoffset, k.yoffset); \n\
                                                                i = 0; \n\
                                                                xtemp = m.x * m.x - m.y * m.y + m.x_origin; \n\
                                                                while (m.x * m.x + m.y * m.y <= 4 && i < k.iter) \n\
                                                                { \n\
                                                                    xtemp = m.x * m.x - m.y * m.y + m.x_origin; \n\
                                                                    m.y = 2 * m.x * m.y + m.y_origin; \n\
                                                                    m.x = xtemp; \n\
                                                                    i++; \n\
                                                                } \n\
                                                                if (i == k.iter) \n\
                                                                    out[(m.width * m.y_dim) + m.x_dim] = 0; \n\
                                                                else \n\
                                                                    out[(m.width * m.y_dim) + m.x_dim] = ft_color(k, m, i); \n\
                                                            } \n\
                                                            ";
    return (Source);
}