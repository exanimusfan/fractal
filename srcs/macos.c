/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macos.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/09 16:44:08 by viccarau          #+#    #+#             */
/*   Updated: 2019/06/14 06:59:40 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

cl_command_queue	cl_cmnd_q(cl_context ctxt, cl_device_id id, cl_int *err)
{
	return (clCreateCommandQueue(ctxt, id, 0, err));
}
