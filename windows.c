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
#define ApplicationTotalMemory (7680 * 4320 * 4) //This is a frame
//                                                 of 8k resolution

#include "fractal.h"
#include "fractal.c"

// NOTE(V Caraulan): Windows header files
#include <windows.h>
#include <wingdi.h>
#include <xinput.h>
#include <io.h>
#include <fileapi.h>
#include <gl/gl.h>

#include "windows_layer.h"

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, \
XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);

X_INPUT_GET_STATE(XInputGetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, \
XINPUT_VIBRATION *pVibration)
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

internal inline win32_persistent_storage
InitializeMemory(size_t Size)
{
    win32_persistent_storage Result = {0};
    
    Result.UsedSize = 0;
    Result.TotalSize = ApplicationTotalMemory;
    if (Size > Result.TotalSize) // TODO(V Caraulan): Potential realloc later
        Result.TotalSize = Size;
    Result.Memory = VirtualAlloc(0, Result.TotalSize,
                                 MEM_RESERVE | MEM_COMMIT,
                                 PAGE_READWRITE);
    if (!Result.Memory)
    {
        MessageBox(NULL, "Can not initialize memory\n", NULL,
                   MB_ICONEXCLAMATION | MB_YESNO);
        *(char *)(0) = 1; //NOTE: CRASH HARD TODO
    }
    return (Result);
}

internal void
Win32ResizeDIBSection(win32_persistent_storage Storage,
                      win32_offscreen_buffer *Buffer,
                      win32_window_dimension Dimension)
{
    Buffer->Memory = Storage.Memory;
    Buffer->Width = Dimension.Width;
    Buffer->Height = Dimension.Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
    
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, win32_window_dimension Dimension,
                           win32_offscreen_buffer Buffer)
{
    StretchDIBits(DeviceContext,
                  0, 0, Dimension.Width, Dimension.Height,
                  0, 0,    Buffer.Width,    Buffer.Height,
                  Buffer.Memory, &Buffer.Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message,
                   WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            OutputDebugStringA("ERROR\n");
            MessageBox(NULL, "You shouldn't be able to lose key presses\n", NULL,
                       MB_ICONEXCLAMATION | MB_YESNO);
            *(char *)(0) = 1; //NOTE: CRASH HARD TODO
            
        } break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }
    }
    return (Result);
}

global_variable s64 GlobalPerfCountFrequency;

internal inline LARGE_INTEGER
Win32GetWallClock(void)
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return (Result);
}

internal inline f32
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    f32 Result = (f32)((End.QuadPart - Start.QuadPart) / (f32) GlobalPerfCountFrequency);
    return (Result);
}

internal inline u64
CheckKeyPress(int IsDown, u64 KeyPress, int Flag)
{
    if (IsDown)
        KeyPress |= (u64)(1UL << Flag);
    else
        KeyPress &= ~(1UL << Flag);
    return (KeyPress);
}

int
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
        LPSTR CommanLine, int ShowCode)
{
    WNDCLASS WindowClass      = {0};
    HWND Window               = NULL;
    WindowClass.style         = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc   = MainWindowCallback;
    WindowClass.hInstance     = Instance;
    WindowClass.lpszClassName = "FractalWindowClass";
    
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    
    u8 DesiredSchedulerMS = 1;
    BOOL SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
    
    //int MonitorRefreshHz = 60;
    //int ApplicationHz = MonitorRefreshHz / 2;
    int ApplicationHz = 30;
    f32 TargetSecondsPerFrame = 1.0f / (f32)ApplicationHz;
    f64 test = 1.0f; // TODO(V Caraulan): Remove this
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
        win32_window_dimension   Resolution = GetWindowDimension(Window);
        win32_persistent_storage Storage = InitializeMemory(Resolution.Width  *
                                                            Resolution.Height * 4);
        win32_offscreen_buffer   Buffer = {0};
        Win32ResizeDIBSection(Storage, &Buffer, Resolution);
        int Running = 1;
        application_input_handle Input = {0};
        
        // NOTE(V Caraulan): Begining of profiling
        LARGE_INTEGER LastCounter = Win32GetWallClock();
        u64 LastCycleCount = __rdtsc();
        
        f32 MiliSecondsSinceLastInput = 0.0f;
        
        BOOL MouseIsDown = 0;
        BOOL MouseWasDown = 0;
        int xoffset = 0;
        int yoffset = 0;
        while (Running)
        {
            MSG Message;
            while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE))
            {
                switch(Message.message)
                {
                    case WM_DESTROY:
                    {
                        Running = 0;
                        PostQuitMessage(0);
                    } break;
                    
                    case WM_QUIT:
                    {
                        Running = 0;
                        PostQuitMessage(0);
                    } break;
                    case WM_SIZE:
                    {
                        MiliSecondsSinceLastInput = 0.0f;
                        win32_window_dimension Dimension = GetWindowDimension(Window);
                        Win32ResizeDIBSection(Storage, &Buffer, Dimension);
                    } break;
                    case WM_CLOSE:
                    {
                        Running = 0;
                        PostQuitMessage(0);
                    } break;
                    case WM_MOUSEWHEEL:
                    {
                        MiliSecondsSinceLastInput = 0.0f;
                        Input.MouseWheel -= (short) HIWORD(Message.wParam);
                    } break;
                    
                    case WM_LBUTTONUP:
                    {
                        MouseIsDown = 0;
                        MouseWasDown = 1;
                    } break;
                    case WM_LBUTTONDOWN:
                    {
                        MouseWasDown = 0;
                        MouseIsDown = 1;
                    } break;
                    
                    case WM_KEYUP:
                    case WM_KEYDOWN:
                    {
                        int VKCode = (int)Message.wParam;
                        
                        int WasDown = ((Message.lParam & (1 << 30)) != 0);
                        //NOTE: variable names explain the bit shift
                        int IsDown = ((Message.lParam & (1 << 31)) == 0);
                        
                        MiliSecondsSinceLastInput = 0.0f;
                        if (IsDown != WasDown)
                        {
                            switch(VKCode)
                            {
                                case VK_ESCAPE:
                                {
                                    if (IsDown)
                                        Running = 0;
                                }break;
                                case VK_UP:
                                case 'W':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_W);
                                }break;
                                case VK_DOWN:
                                case 'S':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_S);
                                }break;
                                case VK_LEFT:
                                case 'A':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_A);
                                }break;
                                case VK_RIGHT:
                                case 'D':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_D);
                                }break;
                                case '1':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_1);
                                }break;
                                case '2':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_2);
                                }break;
                                case VK_OEM_MINUS:
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_MINUS);
                                }break;
                                case VK_OEM_PLUS:
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_PLUS);
                                }break;
                                case 'R':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_R);
                                }break;
                                case 'T':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_T);
                                }break;
                                case 'B':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_B);
                                }break;
                                case 'N':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_N);
                                }break;
                                case 'G':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_N);
                                }break;
                                case 'H':
                                {
                                    Input.KeyPress = CheckKeyPress(IsDown,
                                                                   Input.KeyPress,
                                                                   KEY_H);
                                }break;
                                
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
            // NOTE(V Caraulan): Click and drag
            // TODO(V Caraulan): Maybe use flags here ?
            if (MouseIsDown)
            {
                POINT MousePosition;
                
                GetCursorPos(&MousePosition);
                if (xoffset != 0 && yoffset != 0)
                {
                    xoffset -= -MousePosition.x;
                    yoffset -= -MousePosition.y;
                    Input.MouseRelativePos.x = xoffset;
                    Input.MouseRelativePos.y = yoffset;
                }
                
                Input.MousePosition.x = MousePosition.x;
                Input.MousePosition.y = MousePosition.y;
                xoffset = -MousePosition.x;
                yoffset = -MousePosition.y;
                MiliSecondsSinceLastInput = 0.0f;
                MouseWasDown = 1;
#if 0
                char Temp[256];
                sprintf_s(Temp, 256, "x %d, %d, %d, %d\n", Input.MousePosition.x, Input.MousePosition.y, Input.MouseRelativePos.x,
                          Input.MouseRelativePos.y);
                OutputDebugStringA(Temp);
#endif
            }
            else if (!MouseIsDown && MouseWasDown)
            {
                xoffset = 0;
                yoffset = 0;
                MouseWasDown = 0;
            }
            
            for (DWORD ControllerIndex = 0;
                 ControllerIndex < XUSER_MAX_COUNT;
                 ++ControllerIndex)
            {
                XINPUT_STATE ControllerState = {0};
                if (XInputGetState_(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                {
#if 0
                    XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                    int Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    int Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    int Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    int Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                    int BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                    if (BButton)
                        Running = 0;
                    int StickX = Pad->sThumbLX;
                    int StickY = Pad->sThumbLY;
#endif
                }
            }
            
            win32_window_dimension Dimension = GetWindowDimension(Window);
            
            int Render = 0;
            
            if (MiliSecondsSinceLastInput >= 250.0f && !(Buffer.Width == Dimension.Width && Buffer.Height == Dimension.Height))
            {
                Render = 1;
                Win32ResizeDIBSection(Storage, &Buffer, Dimension);
            }
            else if (MiliSecondsSinceLastInput >= 0 && MiliSecondsSinceLastInput < 100.0f)
            {
                Render = 1;
                win32_window_dimension RenderResolution = Dimension;
                RenderResolution.Width  = (int)(RenderResolution.Width * test);
                RenderResolution.Height = (int)(RenderResolution.Height * test);
                Win32ResizeDIBSection(Storage, &Buffer, RenderResolution);
            }
            
            Dimension = GetWindowDimension(Window);
            
            application_offscreen_buffer AplicationBuffer = {0};
            
            AplicationBuffer.Memory = Buffer.Memory;
            AplicationBuffer.Width = Buffer.Width;
            AplicationBuffer.Height = Buffer.Height;
            AplicationBuffer.Pitch = Buffer.Pitch;
            AplicationBuffer.BytesPerPixel = Buffer.BytesPerPixel;
            
            ApplicationUpdateAndRender(AplicationBuffer, Input, Render, test);
            Win32DisplayBufferInWindow(DeviceContext, Dimension, Buffer);
            LARGE_INTEGER WorkCounter = Win32GetWallClock();
            f32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);
            
            f32 SecondsElapsedForFrame = WorkSecondsElapsed;
            //MiliSecondsSinceLastInput += SecondsElapsedForFrame * 1000.0f;
            
            if (SecondsElapsedForFrame < (TargetSecondsPerFrame / 2))
                test = 1.0f;
            if (SecondsElapsedForFrame < TargetSecondsPerFrame)
            {
                //OutputDebugStringA("Success\n");
                while (SecondsElapsedForFrame < TargetSecondsPerFrame)
                {
                    if (SleepIsGranular)
                    {
                        DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame - SecondsElapsedForFrame));
                        Sleep(SleepMS);
                    }
                    SecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());
                }
                MiliSecondsSinceLastInput += SecondsElapsedForFrame * 1000.0f;
            }
            else
            {
                test = 0.1f;
                test = ClampF(test, 0.1f, 1.0f);
                MiliSecondsSinceLastInput += SecondsElapsedForFrame * 1000.0f;
                //// TODO(V Caraulan): Missed framerate
            }
            
#if 0
            f32 MSPerFrame = (1000.0f * (f32)CounterElapsed) / (f32)PerfCountFrequency;
            f32 FPS = (f32)PerfCountFrequency / (f32)CounterElapsed;
            f32 MCPF = (f32)(CycleElapsed / (1000.0f * 1000.0f));
            if (MSPerFrame > 0.0f)
                MiliSecondsSinceLastInput += MSPerFrame;
            char TempBuffer[256];
            sprintf_s(TempBuffer, 256, "last input = %f %fMS/f. %fFPS, %fmc/f\b\n",
                      MiliSecondsSinceLastInput, MSPerFrame, FPS, MCPF);
            OutputDebugStringA(TempBuffer);
#endif
            
            Input.MouseWheel = 0;
            LARGE_INTEGER EndCounter = Win32GetWallClock();
            LastCounter = EndCounter;
            
            s64 EndCycleCount = __rdtsc();
            //s64 CycleElapsed = EndCycleCount - LastCycleCount;
            LastCycleCount = EndCycleCount;
            
        }
        ReleaseDC(Window, DeviceContext);
    }
    return (0);
}
