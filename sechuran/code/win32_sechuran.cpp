/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Felipe Carlin $
   $Notice: $
   ======================================================================== */

#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct win32_offscreen_buffer
{
    // NOTE(felipe): Pixels are 32-bit wide
    // Memory Order: BB GG RR XX
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

// TODO(felipe): This is global for now.
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;    
    
    return Result;
}

internal void
DebugRender(win32_offscreen_buffer Buffer, int XOffset,int YOffset)
{
    uint8 *Row = (uint8 *)Buffer.Memory;
    for(int Y = 0;
        Y < Buffer.Height;
        ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X = 0;
            X < Buffer.Width;
            ++X)
        {
            uint8 Red = 40;
            uint8 Green = 40;
            uint8 Blue = 40;

            uint8 Grey = ((Y + YOffset)%100) ? 40 : 120;
            Red = Grey;
            Green = Grey;
            Blue = Grey;
            
            *Pixel++ = ((Red << 16) | (Green << 8) | (Blue << 0));
        }

        Row += Buffer.Pitch;
    }
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    // TODO(felipe): Bulletproof this.
    // Maybe don't free first, free after, then free first if that fails.

    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;

    int BytesPerPixel = 4;
    
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Width*BytesPerPixel;
    
    // TODO(felipe): Maybe clear to black.
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, win32_offscreen_buffer Buffer,
                           int WindowWidth, int WindowHeight)
{
    // TODO(felipe): Filtering.
    StretchDIBits(DeviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  */
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer.Width, Buffer.Height,
                  Buffer.Memory,
                  &Buffer.Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
WindowProc(HWND Window, UINT Message,
           WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
        } break;

        case WM_CLOSE:
        {
            // TODO(felipe): Handle this with a message to the user?
            GlobalRunning = false;
        } break;

        case WM_DESTROY:
        {
            // TODO(felipe): Handle this as an error - recreate window?
            GlobalRunning = false;    
        } break;

        case WM_ACTIVATEAPP:
        {
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(DeviceContext, GlobalBackBuffer,
                                       Dimension.Width, Dimension.Height);
            EndPaint(Window, &Paint);
        } break;
        
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        PSTR CommandLine,
        INT CommandShow)
{
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
    
    WNDCLASS WindowClass = {};

    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = WindowProc;
//    WindowClass.cbClsExtra = ;
//    WindowClass.cbWndExtra = ;
    WindowClass.hInstance = Instance;
//    WindowClass.hIcon = ;
//    WindowClass.hCursor = ;
//    WindowClass.hbrBackground = ;
//    WindowClass.lpszMenuName = ;
    WindowClass.lpszClassName = "SechuranWindowClass";

    if(RegisterClassA(&WindowClass))
    {
        HWND Window =
            CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "Sechuran",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);

        if(Window)
        {
            HDC DeviceContext = GetDC(Window);

            int XOffset = 0;
            int YOffset = 0;
            
            GlobalRunning = true;
            while(GlobalRunning)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    TranslateMessage(&Message); 
                    DispatchMessage(&Message);
                }

                DebugRender(GlobalBackBuffer, XOffset, YOffset);

                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, GlobalBackBuffer,
                                           Dimension.Width, Dimension.Height);

                ++XOffset;
                ++YOffset;
            }
        }
        else
        {
            // TODO(felipe): Logging.
        }
    }
    else
    {
        // TODO(felipe): Logging.
    }
    
    return 0;
}
