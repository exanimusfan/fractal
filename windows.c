/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/*   windows.c                                                                */
/*   By: V Caraulan <caraulan.victor@yahoo.com>                               */
/*   Created: 2021/01/03 11:09:27 by V Caraulan                               */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

#include <math.h>
#include <stdint.h>

#define internal static 
#define global_variable static
#define local_persist static

#include "fractal.h"
#include "fractal.c"

// NOTE(V Caraulan): Windows header files
#include <windows.h>
#include <wingdi.h>
#include <xinput.h>
#include <io.h>
#include <fileapi.h>

#if 0
void WINAPIV DebugOut(const TCHAR *fmt, ...) {
    TCHAR s[4096];
    va_list args;
    va_start(args, fmt);
    wvsprintf(s, fmt, args);
    va_end(args);
    OutputDebugString(s);
}
#endif

#if Profiling
# include <stdio.h>
#endif


#include "windows_layer.h"

global_variable  int GlobalRunning; // TODO(V Caraulan): Have it inside a struct, or the application shouldn't even be aware about this variable ??? I like the latter better

global_variable  win32_offscreen_buffer GlobalBuffer;

// TODO(V Caraulan): Remove the global variables !!!
// TODO(V Caraulan): Maybe controller as input ???
// TODO(V Caraulan): What should happen in case of resizing ? Right now we crash hard or soft

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);

X_INPUT_GET_STATE(XInputGetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED); // The original function returns 0 on success
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;

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

internal win32_window_dimension
GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return (Result);
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
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, win32_window_dimension Dimension,
                           win32_offscreen_buffer Buffer)
{
    if (Buffer.LowResolution)
    {
        Buffer.Info.bmiHeader.biWidth = Buffer.Width / 10;
        Buffer.Info.bmiHeader.biHeight = -(Buffer.Height / 10);
        StretchDIBits(DeviceContext,
                      0, 0, Dimension.Width, Dimension.Height,
                      0, 0, Buffer.Width / 10, Buffer.Height / 10,
                      Buffer.Memory, &Buffer.Info,
                      DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        Buffer.Info.bmiHeader.biWidth = Buffer.Width;
        Buffer.Info.bmiHeader.biHeight = -Buffer.Height;
        StretchDIBits(DeviceContext,
                      0, 0, Dimension.Width, Dimension.Height,
                      0, 0, Buffer.Width , Buffer.Height,
                      Buffer.Memory, &Buffer.Info,
                      DIB_RGB_COLORS, SRCCOPY);
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
            // TODO(V Caraulan): Resize only if buffer is too small ???
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

        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            OutputDebugStringA("ERROR\n");
            *(char *)(0) = 1; //NOTE: CRASH HARD TODO
        } break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            BeginPaint(Window, &Paint);
            EndPaint(Window, &Paint);
        } break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }
    }
    return (Result);
}

int
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
        LPSTR CommanLine, int ShowCode)
{
    WNDCLASS WindowClass      = {0};
    HWND Window               = NULL;
    win32_window_dimension   Resolution;

    Resolution.Width = 1920;
    Resolution.Height = 1080;

    Win32ResizeDIBSection(&GlobalBuffer, Resolution);
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
                                 // Position and Size
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 NULL,       // Parent window    
                                 NULL,       // Menu
                                 WindowClass.hInstance,  // Instance handle
                                 NULL        // Additional application data
                                 );
    }
    if (Window)
    {
        ShowWindow(Window, ShowCode);

        HDC DeviceContext = GetDC(Window);
        GlobalRunning = 1;
        win32_input_handle Input = {0};
#if Profiling
        LARGE_INTEGER LastCounter;
        QueryPerformanceCounter(&LastCounter);
#endif
        while (GlobalRunning)
        {
            MSG Message;
            while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE))
            {
                switch(Message.message)
                {
                    case WM_QUIT:
                    {
                        GlobalRunning = 0;
                    } break;
                    case WM_MOUSEWHEEL:
                    {
                        // TODO(V Caraulan): What if I had a increase, decrease value, depending
                        // on mouse wheel. While value != 0 low resolution else high.
                        //int ScrollDirection = ((short) HIWORD(Message.wParam)< 0) ? -1 : +1;


                    } break;
#if 0
                    case WM_MOUSEMOVE:
                    {
                        int MKCode = WParam;

                        if (MKCode == )
                    } break:
#endif
                    case WM_KEYUP:
                    case WM_KEYDOWN:
                    {
                        GlobalBuffer.LowResolution = 1;
                        int VKCode = (int)Message.wParam;

                        int WasDown = ((Message.lParam & (1 << 30)) != 0);
                        //NOTE: variable names explain the bit shift
                        int IsDown = ((Message.lParam & (1 << 31)) == 0);

                        if (IsDown != WasDown)
                        {
                            if (VKCode == VK_ESCAPE){ //Quit
                                if (IsDown)
                                    GlobalRunning = 0;
                            }
                            if (VKCode == 'W' || VKCode == VK_UP){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_W);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_W);
                            }
                            if (VKCode == 'S' || VKCode == VK_DOWN){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_S);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_S);
                            }
                            if (VKCode == 'A' || VKCode == VK_LEFT){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_A);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_A);
                            }
                            if (VKCode == 'D' || VKCode == VK_RIGHT){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_D);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_D);
                            }
                            if (VKCode == '1'){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_1);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_1);
                            }
                            if (VKCode == '2'){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_2);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_2);
                            }
                            if (VKCode == VK_OEM_MINUS){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_MINUS);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_MINUS);
                            }
                            if (VKCode == VK_OEM_PLUS){
                                if (IsDown)
                                    Input.KeyPress |= (1UL << KEY_PLUS);
                                else
                                    Input.KeyPress &= ~(1UL << KEY_PLUS);
                            }
                        }
                    }break;
                    default:
                    {
                        TranslateMessage(&Message);
                        DispatchMessage(&Message);
                    }
                }
            }
            for (DWORD ControllerIndex = 0;
                 ControllerIndex < XUSER_MAX_COUNT;
                 ++ControllerIndex)
            {
                XINPUT_STATE ControllerState = {0};
                if (XInputGetState_(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                {
                    XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
#if 0
                    int Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    int Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    int Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    int Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                    int BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                    int StickX = Pad->sThumbLX;
                    int StickY = Pad->sThumbLY;
#endif
                    int BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                    if (BButton)
                    {
                        GlobalRunning = 0;
                    }
                }
            }
            application_offscreen_buffer Buffer = {0};

            Buffer.Memory = GlobalBuffer.Memory;
            if (GlobalBuffer.LowResolution)
            {
                Buffer.Width = GlobalBuffer.Width / 10;
                Buffer.Height = GlobalBuffer.Height / 10;
            }
            else
            {
                Buffer.Width = GlobalBuffer.Width;
                Buffer.Height = GlobalBuffer.Height;
            }
            Buffer.Pitch = GlobalBuffer.Pitch;
            Buffer.BytesPerPixel = GlobalBuffer.BytesPerPixel;

            application_input_handle localInput;
            localInput.KeyPress = Input.KeyPress;
            localInput.MouseWheel = Input.MouseWheel;
            ApplicationUpdateAndRender(Buffer, localInput);
            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, Dimension, GlobalBuffer);
#if Profiling
            int64 EndCycleCount = __rdtsc();

            LARGE_INTEGER EndCounter;
            QueryPerformanceCounter(&EndCounter);

            int64 CycleElapsed = EndCycleCount - LastCycleCount;
            int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
            f32 MSPerFrame = (1000.0f * (f32)CounterElapsed) / (f32)PerfCountFrequency;
            f32 FPS = (f32)PerfCountFrequency / (f32)CounterElapsed;
            f32 MCPF = (f32)(CycleElapsed / (1000.0f * 1000.0f));
            if (FPS < 200)
            {
                char TempBuffer[256];
                sprintf(TempBuffer, "%fMS/f. %fFPS, %fmc/f\b\n", MSPerFrame, FPS, MCPF);
                OutputDebugStringA(TempBuffer);
            }

            LastCounter = EndCounter;
            LastCycleCount = EndCycleCount;
#endif
        }
        ReleaseDC(Window, DeviceContext);
    }
    return (0);
}
