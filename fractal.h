/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   fractal.h                                                                */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/02 17:57:19 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#if !defined(FRACTAL_H)

# include <math.h>
# include <CL/cl.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float  f32;
typedef double f64;

// TODO(V Caraulan): Remove stdio
#include <stdio.h>

typedef struct	s_dbug
{
	cl_int			err;
	cl_device_id	devices[16];
	cl_char			vendor_name[1024];
	cl_char			device_name[1024];
}				t_dbug;

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
	double	xmin;
	double	xmax;
	double	ymin;
	double	ymax;
	double	xoffset;
	double	yoffset;
	int		iter;
	double	red;
	double	green;
	double	blue;
}					t_krn;

typedef struct s_2d
{
	double x;
	double y;
}              t_2d;

typedef struct s_fol
{
	int            x;
	int            y;
	t_2d           accel;
	unsigned long  flag;
	int            *img;

    //// // TODO(V Caraulan): separate this struct from this middle ?

    t_ocl          ocl;
	t_krn          k;
	cl_device_type dtype;
}              t_fol;

typedef struct
{
    void *Memory;
    int  Width;
    int  Height;
    int  Pitch;
    int  BytesPerPixel;
}              application_offscreen_buffer;

typedef struct
{
    int   SamplesPerSecond;
    int   SampleCount;
    int16 *Samples;
}              game_sound_output_buffer;

typedef enum 
{
    DEFAULT,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_ESCAPE,
    KEY_R,
    KEY_T,
    KEY_G,
    KEY_H,
    KEY_B,
    KEY_N,
    KEY_A,
    KEY_D,
    KEY_W,
    KEY_S,
    KEY_PLUS,
    KEY_MINUS,
} my_keys;

typedef enum
{
    NONE,
    FLAG_INITIALIZED,
    FLAG_CL_INITIALIZED,
}              my_flag;

typedef struct
{
    uint64 KeyPress;
    uint64 LeftStick;
    uint64 RightStick;
    int32 MouseWheel;
}              application_input_handle;

#define FRACTAL_H
#endif
