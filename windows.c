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

#include <windows.h>
#include <wingdi.h>
#include <xinput.h>
#include <io.h>
#include <fileapi.h>
#include <gl/gl.h>

#include "windows_layer.h"

global_variable int                    GlobalRunning = 1;
global_variable win32_offscreen_buffer GlobalBuffer  = {0};

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

internal HGLRC
Win32InitOpenGL(HWND Window)
{
    HDC WindowDC = GetDC(Window);

    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {0};
    DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
    DesiredPixelFormat.nVersion = 1;
    DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    DesiredPixelFormat.cColorBits = 24;
    DesiredPixelFormat.cAlphaBits = 8;
    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowDC, &DesiredPixelFormat);

    PIXELFORMATDESCRIPTOR SuggestedPixelFormat;
    DescribePixelFormat(WindowDC, SuggestedPixelFormatIndex, sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
    SetPixelFormat(WindowDC, SuggestedPixelFormatIndex, &SuggestedPixelFormat);
    HGLRC OpenGLRC = wglCreateContext(WindowDC);
    wglMakeCurrent(WindowDC, OpenGLRC);
    ReleaseDC(Window, WindowDC);
    return (OpenGLRC);
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
Win32ResizeDIBSection(win32_persistent_storage *Storage,
                      win32_window_dimension Dimension)
{
    GlobalBuffer.Width = Dimension.Width;
    GlobalBuffer.Height = Dimension.Height;
    GlobalBuffer.BytesPerPixel = 4;
    GlobalBuffer.Pitch = GlobalBuffer.Width * GlobalBuffer.BytesPerPixel;

    Storage->UsedSize = Dimension.Width * Dimension.Height
        * GlobalBuffer.BytesPerPixel;

    if (Dimension.Width *
        Dimension.Height *
        GlobalBuffer.BytesPerPixel > Storage->TotalSize)
    {
        Storage->TotalSize = Dimension.Width *
            Dimension.Height * GlobalBuffer.BytesPerPixel;
        if (Storage->Memory)
            VirtualFree(Storage->Memory, 0, MEM_RELEASE);
        Storage->Memory = VirtualAlloc(0, Storage->TotalSize,
                                       MEM_RESERVE | MEM_COMMIT,
                                       PAGE_READWRITE);
    }
    if (!Storage->Memory)
    {
        MessageBox(NULL, "Can not initialize memory\n", NULL,
                   MB_ICONEXCLAMATION | MB_OK);
    }
    GlobalBuffer.Memory = Storage->Memory;
}


internal void
Win32DisplayBufferInWindow(HDC DeviceContext, win32_window_dimension WindowDimension)
{
    glViewport(0, 0, WindowDimension.Width, WindowDimension.Height);

    glBindTexture(GL_TEXTURE_2D, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GlobalBuffer.Width, GlobalBuffer.Height, 0,
                 GL_BGRA_EXT, GL_UNSIGNED_BYTE, GlobalBuffer.Memory);

#if 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_DEPTH_TEXTURE);
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glBegin(GL_TRIANGLES);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(-1, -1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(1 ,1);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(-1, -1);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(1 ,1);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(-1, 1);

    glEnd();
    SwapBuffers(DeviceContext);
}

internal inline void
Win32CheckControllerInput(application_input_handle *Input, f32 *MiliSecondsSinceLastInput)
{
    for (DWORD ControllerIndex = 0;
         ControllerIndex < XUSER_MAX_COUNT;
         ++ControllerIndex)
    {
        XINPUT_STATE ControllerState = {0};
        if (XInputGetState_(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
        {
            Input->MouseRelativePos.x = 0;
            Input->MouseRelativePos.y = 0;
            XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
            int Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
            int Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
            int Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
            int Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

            int BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
            int XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
            int AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
            if (BButton)
                GlobalRunning = 0;
            if (AButton || XButton)
                *MiliSecondsSinceLastInput = 0.0f;
            if(AButton)
                Input->KeyPress |= (1UL << KEY_PLUS);
            else
                Input->KeyPress &= ~(1UL << KEY_PLUS);
            if (XButton)
                Input->KeyPress |= (1UL << KEY_MINUS);
            else
                Input->KeyPress &= ~(1UL << KEY_MINUS);

            int StickX = Pad->sThumbLX;
            int StickY = Pad->sThumbLY;

            float magnitude = (float)sqrt(StickY * StickY + StickX * StickX);
            float normalizedMagnitude = 0;
            float normalizedLX = StickX / magnitude;
            float normalizedLY = StickY / magnitude;

            if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
            {
                if (magnitude > 32767)
                    magnitude = 32767;
                magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
                normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
            }
            else
            {
                magnitude = 0.0;
                normalizedMagnitude = 0.0;
            }
            if (magnitude > 0.0)
            {
                Input->MouseRelativePos.x -= (int)(normalizedLX * 10);
                Input->MouseRelativePos.y -= (int)(normalizedLY * 10);
                *MiliSecondsSinceLastInput = 0.0f;
            }
            else
            {
                Input->MouseRelativePos.x = 0;
                Input->MouseRelativePos.y = 0;
            }
            if (Up || Left || Right || Down)
            {
                Input->MouseRelativePos.x -= (10) * Right;
                Input->MouseRelativePos.x += (10) * Left;
                Input->MouseRelativePos.y += (10) * Up;
                Input->MouseRelativePos.y -= (10) * Down;
                *MiliSecondsSinceLastInput = 0.0f;
            }
            int RStickX = Pad->sThumbRX;
            int RStickY = Pad->sThumbRY;

            magnitude = (float)sqrt(RStickY * RStickY + RStickX * RStickX);
            normalizedMagnitude = 0;
            float normalizedRX = RStickX / magnitude;
            float normalizedRY = RStickY / magnitude;

            if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
            {
                if (magnitude > 32767)
                    magnitude = 32767;
                magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
                normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
            }
            else
            {
                magnitude = 0.0;
                normalizedMagnitude = 0.0;
            }

            if (magnitude > 0.0)
            {
                Input->MouseWheel -= (int)(normalizedRY * 160);
                *MiliSecondsSinceLastInput = 0.0f;
            }
            else
                Input->MouseWheel = 0;
        }
    }
}

LRESULT CALLBACK
MainWindowCallback(HWND Window, UINT Message,
                   WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_DESTROY:
        {
            GlobalRunning = 0;
            PostQuitMessage(0);
        } break;

        case WM_QUIT:
        {
            GlobalRunning = 0;
            DestroyWindow(Window);
        } break;
        case WM_CLOSE:
        {
            GlobalRunning = 0;
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

    // TODO(Victor Caraulan): How to get this from windows ?
    int MonitorRefreshHz = 60;
    int ApplicationHz = MonitorRefreshHz;
    f32 TargetSecondsPerFrame = 1.0f / (f32)ApplicationHz;
    f64 ResolutionPercentage = 1.0f;
    Win32LoadXInput();
    if (RegisterClassA(&WindowClass))
    {
        Window = CreateWindowExA(WindowClass.style,  // Optional window styles.
                                 WindowClass.lpszClassName, // Window class
                                 "Fractal",         // Window text
                                 WS_OVERLAPPEDWINDOW, // Window style
                                 // Position and Size
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 //CW_USEDEFAULT, CW_USEDEFAULT,
                                 1280, 720,
                                 NULL,       // Parent window    
                                 NULL,       // Menu
                                 WindowClass.hInstance,  // Instance handle
                                 NULL        // Additional application data
                                 );
    }
    if (Window)
    {
        //ToggleFullscreen(Window);
        HGLRC glContext = Win32InitOpenGL(Window);
        ShowWindow(Window, ShowCode);
        win32_window_dimension   Resolution = GetWindowDimension(Window);
        win32_persistent_storage Storage = {0};
        Win32ResizeDIBSection(&Storage, Resolution);
        if (!Storage.Memory)
            GlobalRunning = 0;
        application_input_handle Input = {0};

        // NOTE(V Caraulan): Begining of profiling
        LARGE_INTEGER LastCounter = Win32GetWallClock();

        f32 MiliSecondsSinceLastInput = 0.0f;

        BOOL MouseIsDown = 0;
        BOOL MouseWasDown = 0;
        int xoffset = 0;
        int yoffset = 0;
        while (GlobalRunning)
        {
            Win32CheckControllerInput(&Input, &MiliSecondsSinceLastInput);

            MSG Message;
            while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE))
            {
                switch(Message.message)
                {
                    case WM_SIZE:
                    {
                        MiliSecondsSinceLastInput = 0.0f;
                        win32_window_dimension Dimension = GetWindowDimension(Window);
                        Win32ResizeDIBSection(&Storage, Dimension);
                        if (!Storage.Memory)
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

                        if (VKCode == VK_OEM_PLUS || VKCode == VK_OEM_MINUS)
                            MiliSecondsSinceLastInput = 0.0f;
                        if (IsDown != WasDown)
                        {
                            switch(VKCode)
                            {
                                case VK_ESCAPE:
                                {
                                    if (IsDown)
                                        GlobalRunning = 0;
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
            POINT MousePosition;

            GetCursorPos(&MousePosition);
            ScreenToClient(Window, &MousePosition);
            Input.MousePosition.x = MousePosition.x;
            Input.MousePosition.y = MousePosition.y;
            if (MouseIsDown)
            {
                win32_window_dimension Dimension = GetWindowDimension(Window);
                if (MousePosition.x > Dimension.Width || MousePosition.x <= 0 ||
                    MousePosition.y > Dimension.Height || MousePosition.y <= 0)
                {
                    MouseWasDown = 1;
                    MouseIsDown = 0;
                }
                else
                {
                    char TempBuffer[256];
                    sprintf_s(TempBuffer, 256, "%d %d \n", MousePosition.x, MousePosition.y);
                    OutputDebugStringA(TempBuffer);
                    if (xoffset != 0 && yoffset != 0)
                    {
                        xoffset -= -MousePosition.x;
                        yoffset -= -MousePosition.y;
                        Input.MouseRelativePos.x = xoffset;
                        Input.MouseRelativePos.y = -yoffset;
                    }
                    xoffset = -MousePosition.x;
                    yoffset = -MousePosition.y;
                    if (Input.MouseRelativePos.x != 0 || Input.MouseRelativePos.y != 0)
                        MiliSecondsSinceLastInput = 0.0f;
                    MouseWasDown = 1;
                }
            }
            else if (!MouseIsDown && MouseWasDown)
            {
                xoffset = 0;
                yoffset = 0;
                Input.MouseRelativePos.x = 0;
                Input.MouseRelativePos.y = 0;
                MouseWasDown = 0;
            }

            win32_window_dimension Dimension = GetWindowDimension(Window);

            int Render = 0;

            if (MiliSecondsSinceLastInput >= 250.0f && !(GlobalBuffer.Width == Dimension.Width && GlobalBuffer.Height == Dimension.Height))
            {
                Render = 1;
                ResolutionPercentage = 1.0f;
                Win32ResizeDIBSection(&Storage, Dimension);
                if (!Storage.Memory)
                {
                    Render = 0;
                    PostQuitMessage(0);
                }
            }
            else if (MiliSecondsSinceLastInput >= 0 && MiliSecondsSinceLastInput < 100.0f)
            {
                Render = 1;
                win32_window_dimension RenderResolution = Dimension;
                RenderResolution.Width  = (int)(RenderResolution.Width * ResolutionPercentage);
                RenderResolution.Height = (int)(RenderResolution.Height * ResolutionPercentage);
                Win32ResizeDIBSection(&Storage, RenderResolution);
                if (!Storage.Memory)
                {
                    Render = 0;
                    PostQuitMessage(0);
                }
            }

            if ((GlobalBuffer.Width *
                 GlobalBuffer.Height *
                 GlobalBuffer.BytesPerPixel) > Storage.TotalSize)
                Win32ResizeDIBSection(&Storage, Dimension);
            application_offscreen_buffer AplicationBuffer = {0};

            AplicationBuffer.Memory = GlobalBuffer.Memory;
            AplicationBuffer.Width = GlobalBuffer.Width;
            AplicationBuffer.Height = GlobalBuffer.Height;
            AplicationBuffer.Pitch = GlobalBuffer.Pitch;
            AplicationBuffer.BytesPerPixel = GlobalBuffer.BytesPerPixel;

            ApplicationUpdateAndRender(AplicationBuffer, Input, Render, ResolutionPercentage, glContext);

            HDC DeviceContext = GetDC(Window);
            Win32DisplayBufferInWindow(DeviceContext, Dimension);

            LARGE_INTEGER WorkCounter = Win32GetWallClock();
            f32 WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);

            f32 SecondsElapsedForFrame = WorkSecondsElapsed;

            if (SecondsElapsedForFrame < (TargetSecondsPerFrame / 2))
                ResolutionPercentage = 1.0f;
            if (SecondsElapsedForFrame < TargetSecondsPerFrame)
            {
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
                //// NOTE(V Caraulan): Missed framerate
                ResolutionPercentage *= 0.1f;
                ResolutionPercentage = ClampF(ResolutionPercentage, 0.05f, 1.0f);
                MiliSecondsSinceLastInput += SecondsElapsedForFrame * 1000.0f;
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
            ReleaseDC(Window, DeviceContext);

        }
    }
    return (0);
}
