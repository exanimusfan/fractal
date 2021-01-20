/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyspress.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/31 20:15:36 by viccarau          #+#    #+#             */
/*   Updated: 2019/10/23 02:46:15 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"
#include "fractal.h"

static void	acceleration_movement(t_fol *fol)
{
    int	neg = 0;

    if (fol->keypress & 1UL << KEY_W || fol->keypress & 1UL << KEY_S)
	{
		neg = (fol->keypress & 1UL << 16) ? -1 : 1;
		fol->accel.y += neg;
        fol->k.yoffset += ((fol->k.ymax) * neg * 0.005f);
    }
	if (fol->keypress & 1UL << KEY_A || fol->keypress & 1UL << KEY_D)
	{
		neg = (fol->keypress & 1UL << 14) ? -1 : 1;
		fol->accel.x += neg;
        fol->k.xoffset += ((fol->k.xmax) * neg * 0.005f);
    }
	fol->accel.x -= fol->accel.x / 2;
    fol->accel.y -= fol->accel.y / 2;
#if 1
    if (fol->accel.x < 0.00001f && fol->accel.x >= 0.0f)
        fol->accel.x = 0;
    else if (fol->accel.x > -0.00001f && fol->accel.x <= 0.0f)
        fol->accel.x = 0;
    if (fol->accel.y < 0.00001f && fol->accel.y > 0.0f)
        fol->accel.y = 0;
    else if (fol->accel.y > -0.00001f && fol->accel.y <= 0.0f)
        fol->accel.y = 0;
#endif

}

float ClampF(float value, float min, float max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return (value);
}

void	check_keypress(t_fol *fol)
{
    int i;

    i = 0;
    acceleration_movement(fol);
	if (fol->keypress & (1UL << KEY_1))
		fol->k.iter = 1000;
	if (fol->keypress & (1UL << KEY_2))
		fol->k.iter = 10000;
	if (fol->keypress & 1UL << KEY_PLUS)
		fol->k.iter += 100;
	if (fol->keypress & 1UL << KEY_MINUS)
        fol->k.iter -= 100;
	if (fol->keypress & 1UL << KEY_ESCAPE)
        ft_exit(fol);
	if (fol->keypress & 1UL << KEY_R)
		fol->k.red += fol->k.red / 1000;
	if (fol->keypress & 1UL << KEY_T)
		fol->k.red -= fol->k.red / 1000;
	if (fol->keypress & 1UL << KEY_G)
		fol->k.green += fol->k.green / 1000;
	if (fol->keypress & 1UL << KEY_H)
		fol->k.green -= fol->k.green / 1000;
	if (fol->keypress & 1UL << KEY_B)
		fol->k.blue += fol->k.blue / 1000;
	if (fol->keypress & 1UL << KEY_N)
		fol->k.blue -= fol->k.blue / 1000;

    fol->k.red = ClampF(fol->k.red, 0.001f, 0.5f);
    fol->k.green = ClampF(fol->k.green, 0.001f, 0.5f);
    fol->k.blue = ClampF(fol->k.blue, 0.001f, 0.5f);
}
