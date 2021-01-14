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

void		print_stuff(const char *vendor_name, const char *device_name, int i)
{
	ft_putstr("Device : ");
	ft_putnbr(i);
	ft_putchar(' ');
	ft_putstr((const char *)vendor_name);
	ft_putchar(' ');
	ft_putstr((const char *)device_name);
	ft_putchar('\n');
}


void		check_key(t_fol *fol, SDL_Event event,
				SDL_Scancode key, int bit)
{
	if (event.key.keysym.scancode == key)
	{
		if (event.type == SDL_KEYDOWN)
			fol->keypress |= bit;
		else
			fol->keypress &= ~(bit);
	}
}


static void	acceleration_movement(t_fol *fol)
{
		int	neg;

	if (fol->keypress & 1UL << 15 || fol->keypress & 1UL << 16)
	{
		neg = (fol->keypress & 1UL << 16) ? 1 : -1;
		fol->accel.y += neg;
	}
	if (fol->keypress & 1UL << 13 || fol->keypress & 1UL << 14)
	{
		neg = (fol->keypress & 1UL << 14) ? 1 : -1;
		fol->accel.x += neg;
		}
	fol->accel.x -= fol->accel.x / 20;
		fol->accel.y -= fol->accel.y / 20;
	}

void	check_keypress(t_fol *fol)
{
	acceleration_movement(fol);
	if (fol->keypress & 1UL)
		fol->k.iter = 300;
	if (fol->keypress & 1UL << 1)
		fol->k.iter = 500;
	if (fol->keypress & 1UL << 2)
		fol->k.iter = 1000;
	if (fol->keypress & 1UL << 3)
		fol->k.iter = 2000;
	if (fol->keypress & 1UL << 4)
		fol->k.iter = 3000;
	if (fol->keypress & 1UL << 5)
		fol->k.iter = 4000;
	if (fol->keypress & 1UL << 6)
		ft_exit(fol);
	if (fol->keypress & 1UL << 7)
		fol->k.red += fol->k.red / 1000;
	if (fol->keypress & 1UL << 8)
		fol->k.red -= fol->k.red / 1000;
	if (fol->keypress & 1UL << 9)
		fol->k.green += fol->k.green / 1000;
	if (fol->keypress & 1UL << 10)
		fol->k.green -= fol->k.green / 1000;
	if (fol->keypress & 1UL << 11)
		fol->k.blue += fol->k.blue / 1000;
	if (fol->keypress & 1UL << 12)
		fol->k.blue -= fol->k.blue / 1000;
	if (fol->k.red < 0.001f)
		fol->k.red = 0.001;
	if (fol->k.green < 0.001f)
		fol->k.green = 0.001;
	if (fol->k.blue < 0.001f)
		fol->k.blue = 0.001;
	if (fol->k.red > 0.5)
		fol->k.red = 0.5;
	if (fol->k.green > 0.5)
		fol->k.green = 0.5;
	if (fol->k.blue > 0.5)
		fol->k.blue = 0.5;
	fol->k.xoffset += fol->accel.x * fol->zoom * 0.0001;
	fol->k.yoffset += fol->accel.y * fol->zoom * 0.0001;
}

static int	key_press(SDL_Event event, t_fol *fol)
{
	check_key(fol, event, SDL_SCANCODE_1, 1UL);
	check_key(fol, event, SDL_SCANCODE_2, 1UL << 1);
	check_key(fol, event, SDL_SCANCODE_3, 1UL << 2);
	check_key(fol, event, SDL_SCANCODE_4, 1UL << 3);
	check_key(fol, event, SDL_SCANCODE_5, 1UL << 4);
	check_key(fol, event, SDL_SCANCODE_6, 1UL << 5);
	check_key(fol, event, SDL_SCANCODE_ESCAPE, 1UL << 6);
	check_key(fol, event, SDL_SCANCODE_R, 1UL << 7);
	check_key(fol, event, SDL_SCANCODE_T, 1UL << 8);
	check_key(fol, event, SDL_SCANCODE_G, 1UL << 9);
	check_key(fol, event, SDL_SCANCODE_H, 1UL << 10);
	check_key(fol, event, SDL_SCANCODE_B, 1UL << 11);
	check_key(fol, event, SDL_SCANCODE_N, 1UL << 12);
	check_key(fol, event, SDL_SCANCODE_A, 1UL << 13);
	check_key(fol, event, SDL_SCANCODE_D, 1UL << 14);
	check_key(fol, event, SDL_SCANCODE_W, 1UL << 15);
	check_key(fol, event, SDL_SCANCODE_S, 1UL << 16);
	//if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
		//else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
		return (0);
}

int			key_func(SDL_Event event, t_fol *fol)
{
	key_press(fol->sdl.event, fol);
		if (event.key.keysym.scancode == SDL_SCANCODE_EQUALS)
		fol->k.iter++;
	if (event.key.keysym.scancode == SDL_SCANCODE_MINUS && fol->k.iter > 100)
		fol->k.iter--;
	mouse(fol, fol->sdl.event);
		return (0);
}

int		mouse_mv(t_fol *fol, SDL_Event event)
{
	if (event.type == SDL_MOUSEMOTION)
	{
		fol->xx = event.motion.x;
		fol->yy = event.motion.y;
		if (fol->flag & (1UL))
		{
			fol->x = event.motion.x;
			fol->y = event.motion.y;
		}
	}
	if (event.type == SDL_MOUSEBUTTONDOWN)
{
		if (event.button.button == SDL_BUTTON_RIGHT)
	fol->flag ^= 1UL;
	}
	return (0);
}

int		mouse(t_fol *fol, SDL_Event event)
{
	//motion = (t_f64)(event.motion.xrel * 0.001f);
	mouse_mv(fol, event);
	if (event.type == SDL_MOUSEWHEEL)
	{
		if (event.wheel.y > 0)
			fol->zoom *= 1.1f;
			else if (event.wheel.y < 0)
			fol->zoom *= 0.9f;
		fol->k.xoffset += (((double)fol->xx / W * 3.5f) - 2.5f) * (fol->zoom * 0.1f);
		fol->k.yoffset += (((double)fol->yy / H * 2.5f) - 1.25f) * (fol->zoom * 0.1f);
		fol->k.xmax = 3.5f * fol->zoom;
		fol->k.xmin = 2.5f * fol->zoom;
		fol->k.ymax = 2.5f * fol->zoom;
		fol->k.ymin = 1.25f * fol->zoom;
	}
	return (0);
	}
