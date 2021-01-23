/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   macos.c                                                                  */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/21 18:00:17 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#include "fractol.h"

cl_command_queue	cl_cmnd_q(cl_context ctxt, cl_device_id id, cl_int *err)
{
	return (clCreateCommandQueue(ctxt, id, 0, err));
}
