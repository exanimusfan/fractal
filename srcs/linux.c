/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linux.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/09 00:44:19 by viccarau          #+#    #+#             */
/*   Updated: 2019/10/23 02:30:26 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

cl_command_queue	cl_cmnd_q(cl_context ctxt, cl_device_id id, cl_int *err)
{
	return (clCreateCommandQueueWithProperties(ctxt, id, 0, err));
}
