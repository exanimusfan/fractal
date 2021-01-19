/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractol.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/19 19:16:03 by viccarau          #+#    #+#             */
/*   Updated: 2019/10/23 02:42:39 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTOL_H
# define FRACTOL_H
# include <math.h>
# include "cl_helper.h"
# define RGB_W 0xFFFFFFFF

/*
**	BONUSES :
**	-Can choose between CPU and GPU
**	-You Can shift the colors r g b
**	-Used OpenCL
**	-Can move with arrows, WASD and mouse clicks
**	-Zoom follows the actual mouse position
**	-Can increase and decrease iterations with - and =
**	-Multithreading and posible multi-gpu implementation
**	-Implemented a zoom out depending on mouse movement
**	-It is compatible with linux and macOS assuming that the
**	proper libraries are installed (X11, openCL and Xext on linux)
**	-Added the UI that explains a lot of the functionalities of the keys
*/

typedef struct		s_args
{
	const char	*args[18];
}					t_args;

typedef struct		s_img
{
	int		bpx;
	int		size_line;
	int		endian;
}					t_img;

typedef struct		s_ocl
{
	cl_command_queue	cmd_queue[16];
	cl_device_id		devices[16];
	cl_kernel			krnl;
	cl_context			context;
	cl_int				err;
	cl_uint				num_devices;
	cl_mem				image;
	size_t				buff_size;
	int					exit;
}					t_ocl;

typedef struct		s_krn
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
}					t_krn;

typedef struct s_2d
{
	float x;
	float y;
}              t_2d;

typedef struct s_fol
{
	int            time;
	int            time1;
	float         dt;
	int            totaltime;
	float         zoom;
	t_args         a;
	int            keypress;
    int            x;
	int            y;
	int            test;
    t_2d           accel;
	unsigned long  flag;
	unsigned long  frac;
	int            *img;
	t_ocl          ocl;
	t_krn          k;
	cl_device_type dtype;
    t_2d           old_buffer_size;
}              t_fol;

void				check_keypress(t_fol *fol);
//int					mouse(t_fol *fol, SDL_Event event);
t_args				init_args(void);
t_args				init_kernel_args(void);
int					ft_exit(t_fol *f);
void				ui_function(t_fol *fol);
void				print_stuff(const char *vendor_name,
                    const char *device_name, int i);
int					run_cl(application_offscreen_buffer Buffer, t_fol *fol);
//int					key_func(SDL_Event event, t_fol *fol);
int					mouse_hook(int button, int x, int y, t_fol *fol);
unsigned int		ft_pow2(unsigned int a);
int					rgb_lerp(int color1, float t, int color2);
void				julia(t_fol *f, int x, int y);
int					finish_cl(cl_context context,
                  cl_command_queue cmd_queue, cl_mem image);
cl_command_queue	cl_cmnd_q(cl_context ctxt, cl_device_id id, cl_int *err);
int					is_alloc(void *memory, const char *str);
#endif
