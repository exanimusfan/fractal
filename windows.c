/*   windows.c                                                                */
/*   By: Victor Caraulan <victor.caraulan@yahoo.com>                          */
/*   Created: 2021/01/03 11:09:27 by V Caraulan                               */

#include <math.h>
#include <stdint.h>

#define PI32 3.14159265359
#define internal static 
#define global_variable static
#define local_persist static

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

// NOTE(V Caraulan): Windows header files

#include <windows.h>
#include <wingdi.h>
#include <xinput.h>
#include <dsound.h>
#include <io.h>
#include <fileapi.h>

#if 1
void WINAPIV DebugOut(const TCHAR *fmt, ...) {
    TCHAR s[4096];
    va_list args;
    va_start(args, fmt);
    wvsprintf(s, fmt, args);
    va_end(args);
    OutputDebugString(s);
}
#endif

#include "windows_layer.h"
#include "fractal.h"
#include "fractal.c"

// NOTE(V Caraulan): This is my debugging printf from google

global_variable  int GlobalRunning;
global_variable  win32_offscreen_buffer GlobalBuffer;
global_variable  int KeyPress;

// TODO(V Caraulan): Remove the global variables !!!

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);

X_INPUT_GET_STATE(XInputGetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);


void
Win32InitDSound(HWND Window, win32_sound_output *SoundOutput)
{
    HMODULE DirectSoundLibrary = LoadLibraryA("dsound.dll");

    if (DirectSoundLibrary)
    {
        direct_sound_create *DirectSoundCreate =
            (direct_sound_create *)GetProcAddress(DirectSoundLibrary, "DirectSoundCreate");

        LPDIRECTSOUND DirectSound;

        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
        {
            WAVEFORMATEX WaveFormat = {0};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SoundOutput->SamplesPerSecond;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
            WaveFormat.cbSize = 0;

            if (SUCCEEDED(IDirectSound_SetCooperativeLevel(DirectSound, Window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC BufferDescription = {0};

                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if (SUCCEEDED(IDirectSound_CreateSoundBuffer(DirectSound, &BufferDescription, &PrimaryBuffer, 0)))
                {
                    if (SUCCEEDED(IDirectSoundBuffer_SetFormat(PrimaryBuffer, &WaveFormat)))
                    {
                        //OutputDebugString("Success\n");
                    }
                }

            }
            DSBUFFERDESC BufferDescription = {0};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = SoundOutput->BufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;

            HRESULT Error = IDirectSound_CreateSoundBuffer(DirectSound, &BufferDescription, &SoundOutput->Buffer, 0);
            if (SUCCEEDED(Error))
            {
                //OutputDebugString("This is a test\n");
            }
        }
    }
}

internal char *
Win32OpenAndReadFile(const char *filename, char *source)
{
    OVERLAPPED Overlaped = {0};
    OFSTRUCT   FileStruct;
    HANDLE     FileHandle;
    DWORD      BytesRead;

    FileHandle = CreateFileA("opencl/mandelbrot.cl", // File path
                             GENERIC_READ,           // dwDesiredAccess
                             0,                      // dwShareMode
                             NULL,                   // lpSecurityAttributes
                             OPEN_EXISTING,          // dwCreationDisposition
                             FILE_ATTRIBUTE_NORMAL,  // dwFlagsAndAttributes
                             NULL);                  // hTemplateFile (optional, and ignored in case of opening existing)
    LARGE_INTEGER FileSize;
    GetFileSizeEx(FileHandle, &FileSize);
    uint32 FileSize32 = (int32)FileSize.QuadPart;
    ReadFile(FileHandle, source, FileSize32, &BytesRead, 0);
    source[BytesRead] = '\0';
    CloseHandle(FileHandle);
    return (source);
}

internal char *load_program_source(const char *filename, char *source)
{
    return (Win32OpenAndReadFile(filename, source));
}

internal void
Win32LoadXInput(void)
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (!XInputLibrary)
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if(XInputLibrary)
    {
        XInputGetState_ = (x_input_get_state *)GetProcAddress(XInputLibrary,
                                                              "XInputGetState");
        XInputSetState_ = (x_input_set_state *)GetProcAddress(XInputLibrary,
                                                              "XInputSetState");

    }
}

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

win32_window_dimension
GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return (Result);
}

win32_sound_output
GetDefaultSoundOutput(void)
{
    win32_sound_output SoundOutput = {0};

    SoundOutput.SamplesPerSecond = 48000;
    SoundOutput.ToneHz = 256;
    SoundOutput.ToneVolume = 3000;
    SoundOutput.BytesPerSample = (sizeof(int16) * 2);
    SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
    SoundOutput.BufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
    return (SoundOutput);
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, win32_window_dimension Dimension)
{
    if (Buffer->Memory)
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);

    Buffer->Width = Dimension.Width;
    Buffer->Height = Dimension.Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BytesPerPixel = 4;
    int BitmapMemorySize = Buffer->Width * Dimension.Height * BytesPerPixel;

    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;

    //DebugOut("This has been resized\n");
}



internal void
Win32DisplayBufferInWindow(HDC DeviceContext, win32_window_dimension Dimension, win32_offscreen_buffer Buffer, int X, int Y)
{
    int LowHighResolution = 0;
    if (LowHighResolution == 0)
    {
        StretchDIBits(DeviceContext,
                      0, 0, Buffer.Width, Buffer.Height,
                      0, 0, Dimension.Width , Dimension.Height,
                      Buffer.Memory, &Buffer.Info,
                      DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        StretchDIBits(DeviceContext,
                      0, 0, Buffer.Width, Buffer.Height,
                      0, 0, Dimension.Width * 0.5, Dimension.Height * 0.5,
                      Buffer.Memory, &Buffer.Info,
                      DIB_RGB_COLORS, SRCCOPY);
    }
}

internal void
Win32FillSoundBuffer(win32_sound_output *SoundOutput, int ByteToLock, int BytesToWrite, uint32 *RunningSampleIndex)
{
    void *Region1;
    DWORD Region1Size;
    void *Region2;
    DWORD Region2Size;

    if (SUCCEEDED(IDirectSoundBuffer_Lock(SoundOutput->Buffer, ByteToLock,
                                          BytesToWrite,
                                          &Region1, &Region1Size,
                                          &Region2, &Region2Size,
                                          0)))
    {
        //assert that region1 is valid
        game_sound_output_buffer ApplicationSound = {0};
        ApplicationSound.SamplesPerSecond = SoundOutput->SamplesPerSecond;
        ApplicationSound.SampleCount = Region1Size / SoundOutput->BytesPerSample;
        ApplicationSound.Samples = (int16 *)Region1;
        GameOutputSound(&ApplicationSound, SoundOutput->ToneHz);
        ApplicationSound.SampleCount = Region2Size / SoundOutput->BytesPerSample;
        ApplicationSound.Samples = (int16 *)Region2;
        GameOutputSound(&ApplicationSound, SoundOutput->ToneHz);
        IDirectSoundBuffer_Unlock(SoundOutput->Buffer,
                                  &Region1, Region1Size,
                                  &Region2, Region2Size);
    }
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message,
                   WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_SIZE:
        {
            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32ResizeDIBSection(&GlobalBuffer, Dimension);
        } break;
        case WM_CLOSE:
        {
            GlobalRunning = 0;
            PostQuitMessage(0);
        } break;
        case WM_ACTIVATEAPP:
        {

        } break;
        case WM_DESTROY:
        {
            GlobalRunning = 0;
        } break;
        case WM_MOUSEWHEEL:
        {
            KeyPress |= 1UL << 17;
            int ScrollDirection = ((short) HIWORD(WParam)< 0) ? -1 : +1;
            if (ScrollDirection < 1)
                KeyPress |= 1UL << 18;
            //DebugOut("keypress %d\n", KeyPress);
            GlobalBuffer.LowHighResolution = 1;
        } break;
#if 0
        case WM_MOUSEMOVE:
        {
            GlobalBuffer.LowHighResolution = 1;
            int MKCode = WParam;

            if (MKCode == )
        } break:
#endif
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            int VKCode = WParam;
            int WasDown = ((LParam & (1 << 30)) != 0);
            int IsDown = ((LParam & (1 << 31)) == 0);
            if (IsDown)
                GlobalBuffer.LowHighResolution = 1;
            if (IsDown != WasDown)
            {
                if (VKCode == VK_ESCAPE){ //Quit
                    if (IsDown)
                    {
                        GlobalRunning = 0;
                    }
                }
                if (VKCode == 'W'){ //Up
                    if (IsDown)
                    {
                        KeyPress |= (1UL << 15);
                    }
                    else
                    {
                        KeyPress &= ~(1UL << 15);
                    }
                }
                if (VKCode == 'S'){ //Down
                    if (IsDown)
                    {
                        KeyPress |= (1UL << 16);
                    }
                    else
                    {
                        KeyPress &= ~(1UL << 16);
                    }
                }
                if (VKCode == 'A'){ //Left
                    if (IsDown)
                    {
                        KeyPress |= (1UL << 13);
                    }
                    else
                    {
                        KeyPress &= ~(1UL << 13);
                    }
                }
                if (VKCode == 'D'){ //Right
                    if (IsDown)
                    {
                        KeyPress |= (1UL << 14);
                    }
                    else
                    {
                        KeyPress &= ~(1UL << 14);
                    }
                }
                if (VKCode == 'Q'){ //Right
                    if (IsDown)
                    {
                        KeyPress |= (1UL << 5);
                    }
                    else
                    {
                        KeyPress &= ~(1UL << 5);
                    }
                }
                if (VKCode == 'E'){ //Right
                    if (IsDown)
                    {
                        KeyPress |= (1UL << 6);
                    }
                    else
                    {
                        KeyPress &= ~(1UL << 6);
                    }
                }
            }
        }break;

        case WM_PAINT:
        {
            for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
            {
                XINPUT_STATE ControllerState = {0};
                if (XInputGetState_(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                {
                    XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                    int Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    int Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    int Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    int Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                    int AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                    int BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                    int XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                    int YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

                    int StickX = Pad->sThumbLX;
                    int StickY = Pad->sThumbLY;

                    if (BButton)
                    {
                        GlobalRunning = 0;
                    }
                }
            }
            win32_window_dimension Dimension = GetWindowDimension(Window);
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            Win32DisplayBufferInWindow(DeviceContext, Dimension, GlobalBuffer, 0, 0);
            EndPaint(Window, &Paint);
        } break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }
    }
    return (Result);
}

// TODO(V Caraulan): Remove this include if not using printf ?
#if Profiling
# include <stdio.h>
#endif

int
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
        LPSTR CommanLine, int ShowCode)
{
    WNDCLASS WindowClass      = {0};
    HWND Window               = NULL;

    Win32ResizeDIBSection(&GlobalBuffer, (win32_window_dimension){1280, 720});
    WindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc   = MainWindowCallback;
    WindowClass.hInstance     = Instance;
    WindowClass.lpszClassName = "FractalWindowClass";

#if Profiling
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    int64 LastCycleCount = __rdtsc();
    int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
#endif

    Win32LoadXInput();
    if (RegisterClassA(&WindowClass))
    {
        Window = CreateWindowExA(WindowClass.style,  // Optional window styles.
                                 WindowClass.lpszClassName, // Window class
                                 "Fractal",         // Window text
                                 WS_OVERLAPPEDWINDOW, // Window style
                                 // Size and position
                                 CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
                                 NULL,       // Parent window    
                                 NULL,       // Menu
                                 WindowClass.hInstance,  // Instance handle
                                 NULL        // Additional application data
                                 );
    }
    if (Window)
    {
        ShowWindow(Window, ShowCode);
        // Run the loop.
        int XOffset = 0;
        int YOffset = 0;

        uint32 RunningSampleIndex = 0;

        HDC DeviceContext = GetDC(Window);
        GlobalRunning = 1;

#if Profiling
        LARGE_INTEGER LastCounter;
        QueryPerformanceCounter(&LastCounter);
#endif
        while (GlobalRunning)
        {
            MSG Message;
            BOOL MessageResult = PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE);

            if (MessageResult > 0)
            {
                if (Message.message == WM_QUIT)
                    GlobalRunning = 0;
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            application_offscreen_buffer Buffer = {0};

            Buffer.Memory = GlobalBuffer.Memory;
            Buffer.Width = GlobalBuffer.Width;
            Buffer.Height = GlobalBuffer.Height;
            Buffer.Pitch = GlobalBuffer.Pitch;
            Buffer.BytesPerPixel = GlobalBuffer.BytesPerPixel;

            ApplicationUpdateAndRender(Buffer, &KeyPress);
            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, Dimension, GlobalBuffer, 0, 0);

#if Profiling
            int64 EndCycleCount = __rdtsc();

            LARGE_INTEGER EndCounter;
            QueryPerformanceCounter(&EndCounter);

            int64 CycleElapsed = EndCycleCount - LastCycleCount;
            int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
            f32 MSPerFrame = (1000.0f * (f32)CounterElapsed) / (f32)PerfCountFrequency;
            f32 FPS = (f32)PerfCountFrequency / (f32)CounterElapsed;
            f32 MCPF = (f32)(CycleElapsed / (1000.0f * 1000.0f));

            //DebugOut()
            char TempBuffer[256];
            sprintf(TempBuffer, "%fMS/f. %fFPS, %fmc/f\b\n", MSPerFrame, FPS, MCPF);
            OutputDebugStringA(TempBuffer);

            LastCounter = EndCounter;
            LastCycleCount = EndCycleCount;
#endif
        }
        ReleaseDC(Window, DeviceContext);
    }
    else
    {
        //OutputDebugStringA("hwnd or register class Failed\n");
    }
    return (0);
}
