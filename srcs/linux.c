/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   linux.c                                                                  */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2019/06/09 00:44:19 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#include "fractol.h"

cl_command_queue	cl_cmnd_q(cl_context ctxt, cl_device_id id, cl_int *err)
{
	return (clCreateCommandQueueWithProperties(ctxt, id, 0, err));
}
