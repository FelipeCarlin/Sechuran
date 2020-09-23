/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Felipe Carlin $
   $Notice: $
   ======================================================================== */

#include <windows.h>

LRESULT CALLBACK
WindowProc(HWND Window,
           UINT Message,
           WPARAM WParam,
           LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
            OutputDebugString("WM_SIZE");
        } break;

        case WM_DESTROY:
        {
            OutputDebugString("WM_DESTROY");
        } break;

        case WM_CLOSE:
        {
            OutputDebugString("WM_CLOSE");
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugString("WM_ACTIVATEAPP");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            PatBlt(DeviceContext, X, Y, Width, Height, WHITENESS);
            ReleaseDC(Window, DeviceContext);
        } break;
        
        
    }
    
    Result = DefWindowProcA(Window, Message, WParam, LParam);
    
    return Result;
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        PSTR CommandLine,
        INT CommandShow)
{
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
        HWND Window = CreateWindowExA(0,
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
            for(;;)
            {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);

                if(MessageResult > 0)
                {
                    TranslateMessage(&Message); 
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
            }
        }
    }
    
    return 0;
}
