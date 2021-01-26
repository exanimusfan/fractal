/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   fractal.h                                                                */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/02 17:57:19 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#if !defined(FRACTAL_H)

# include <math.h>
# include <CL/cl.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

typedef struct	s_dbug
{
	cl_int			err;
	cl_device_id	devices[16];
	cl_char			vendor_name[1024];
	cl_char			device_name[1024];
}				t_dbug;

typedef struct		s_OpenCL
{
	cl_command_queue cmd_queue[16];
	cl_device_id     devices[16];
	cl_kernel        Kernel;
	cl_context       context;
	cl_int           err;
	cl_uint          num_devices;
	cl_mem           image;
	size_t           buff_size;
}					t_OpenCL;

typedef struct		s_Kernel
{
	f64	xmin;
	f64	xmax;
	f64	ymin;
	f64	ymax;
	f64	xoffset;
	f64	yoffset;
	s32	iter;
	f64	red;
	f64	green;
	f64	blue;
}					t_Kernel;

typedef struct s_2d
{
	f64 x;
	f64 y;
}              t_2d;

typedef struct s_Position
{
	s32 x;
    s32 y;
}              Position;

typedef struct s_fol
{
	s32            x;
	s32            y;
	t_2d           accel;
	unsigned long  flag;
	s32            *img;

    //// // TODO(V Caraulan): separate this struct from this middle ?

    t_OpenCL          ocl;
	t_Kernel       Kernel;
	cl_device_type dtype;
}              t_fol;

typedef struct
{
    void *Memory;
    s32  Width;
    s32  Height;
    s32  Pitch;
    s32  BytesPerPixel;
}              application_offscreen_buffer;

typedef struct
{
    s32   SamplesPerSecond;
    s32   SampleCount;
    s16 *Samples;
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
    FLAG_INITIALIZED,
    FLAG_CL_INITIALIZED,
}              my_flag;

typedef struct
{
    u64      KeyPress;
    u64      LeftStick;
    u64      RightStick;
    s32      MouseWheel;
    Position MousePosition;
    Position MouseRelativePos;
}              application_input_handle;

#define FRACTAL_H
#endif
