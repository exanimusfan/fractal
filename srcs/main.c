/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viccarau <viccarau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/19 19:15:43 by viccarau          #+#    #+#             */
/*   Updated: 2019/10/23 02:43:16 by viccarau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fractol.h"

static void	mlx_alloc(t_fol *fol)
{
	t_sdl *sdl;

	sdl = &fol->sdl;
	if (fol->frac & 1)
		fol->flag ^= 1UL;
	fol->flag ^= 1UL << 2;
	fol->k.red = 0.002f;
	fol->k.green = 0.003f;
	fol->k.blue = 0.005f;
	fol->zoom = 1;
	fol->k.xmax = 3.5f * fol->zoom;
	fol->k.xmin = 2.5f * fol->zoom;
	fol->k.ymax = 2.5f * fol->zoom;
	fol->k.ymin = 1.25f * fol->zoom;
	fol->k.iter = 1000;
	fol->x = W / 2;
	fol->y = H / 2;
	fol->k.xoffset = 0.74f;
	fol->k.yoffset = 0;
	fol->keypress = 0;
	fol->accel.x = 0;
	fol->accel.y = 0;
	is_alloc(fol->img = ft_memalloc(W * H * sizeof(int)), "Image");
	SDL_Init(SDL_INIT_EVERYTHING);
	sdl->win = SDL_CreateWindow("fractal", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, W, H, 0);
	SDL_SetWindowBordered(sdl->win, SDL_FALSE);
	sdl->renderer = SDL_CreateRenderer(sdl->win, -1, 0);
	sdl->texture = SDL_CreateTexture(sdl->renderer,
                                     SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, W, H);
}

static void ft_putendl(char *str)
{
    int i;

    i = 0;
    while (str[i])
        i++;
    write(1, str, i);
    write(1, "\n", 1);
}
r
static void	ft_usage(char *str)
{
    write(1, "Usage: ", 5);
#if 0
	ft_putstr(str);
	ft_putendl(" [-cg] [name of fractol]");
	ft_putendl("\t-c for CPU multithreaded openCL");
	ft_putendl("\t-g for GPU. Possible multi-GPU if available");
	ft_putendl("Fractols :");
	ft_putendl("\t\"julia\" \"j\"");
	ft_putendl("\t\"mandelbrot\" \"m\"");
	ft_putendl("\t\"mandelbrot2\" \"m2\"");
	ft_putendl("\t\"mandelbrot4\" \"m4\"");
	ft_putendl("\t\"burning ship\" \"b\"");
	ft_putendl("\t\"tricorn\" \"t\"");
	ft_putendl("\t\"menace\" \"ms\"");
	ft_putendl("\t\"burning ship 2\" \"b2\"");
	ft_putendl("\t\"sierpinski\" \"s\"");
#endif
}

static int	choose_cl_device(t_fol *fol, int ac, char **str)
{
	int	i;

	i = 1;
	while (i < ac)
	{
		if (!ft_strcmp(str[i], "-g"))
		{
			fol->dtype = CL_DEVICE_TYPE_GPU;
			return (1);
		}
		else if (!ft_strcmp(str[i], "-c"))
		{
			fol->dtype = CL_DEVICE_TYPE_CPU;
			return (1);
		}
		else if (str[i][0] == '-')
		{
			//ft_putstr("fractol: invalid option -- '");
			//ft_putstr(&str[i][1]);
			//ft_putendl("'");
			ft_exit(fol);
		}
		i++;
	}
	return (1);
}

static int	find_fract(t_fol *fol, char **str, int ac)
{
	int		i;
	int		j;
	int		k;

	k = 0;
	j = 0;
	j = choose_cl_device(fol, ac, str);
	while (k <= 17)
	{
		i = 1;
		while (i < ac)
		{
			if (!ft_strcmp(str[i], fol->a.args[k]))
			{
				fol->frac |= (1UL << k % 9);
				j++;
			}
			i++;
		}
		k++;
	}
	if (j == 2)
		return (1);
	return (0);
}

static void	draw_on_screen(t_fol *fol)
{
	t_sdl *sdl;

	sdl = &fol->sdl;
	SDL_UpdateTexture(sdl->texture, NULL, fol->img,
					  W * sizeof(unsigned int));
	SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
	SDL_RenderPresent(sdl->renderer);
}

int			main(int ac, char **av)
{
	t_fol	fol = {0};

	fol.dtype = CL_DEVICE_TYPE_DEFAULT;
	if (ac >= 1 && ac <= 3)
	{
		if (!find_fract(&fol, av, ac))
			fol.frac |= (1UL << 1);
		mlx_alloc(&fol);
		run_cl(&fol, W / 2, H / 2);
		while (1)
		{
			while (SDL_PollEvent(&fol.sdl.event))
				key_func(fol.sdl.event, &fol);
			check_keypress(&fol);
			if (fol.dt < 1)
			{
				fol.time = SDL_GetTicks();
				run_cl(&fol, fol.x, fol.y);
				draw_on_screen(&fol);
				fol.time1 = SDL_GetTicks();
				fol.dt = fol.time1 - fol.time;
			}
			else
                fol.dt -= fol.dt / 50;
			//printf("%f\t", fol.dt);
		}
	}
	ft_usage(av[0]);
	return (0);
}
