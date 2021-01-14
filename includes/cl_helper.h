/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_helper.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/25 01:35:09 by viccarau          #+#    #+#             */
/*   Updated: 2019/06/16 19:28:27 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CL_HELPER_H
# define CL_HELPER_H
# include <CL/cl.h>
# include "fractol.h"

typedef struct	s_dbug
{
	cl_int			err;
	cl_device_id	devices[16];
	cl_char			vendor_name[1024];
	cl_char			device_name[1024];
}				t_dbug;

cl_kernel		load_krnl(cl_context context, const char *filename);
cl_context		create_context(cl_device_type dtype, cl_uint *num_devices);
void			print_debug_info(cl_context context);
void			check_succeeded(char *message, cl_int err);

#endif
