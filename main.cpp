#include <windows.h>

// 全局变量
HBITMAP g_hBitmap = NULL;
BITMAP g_bitmap;
bool g_bFitToWindow = true;

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            // 加载BMP图片 - 使用Unicode版本
            g_hBitmap = (HBITMAP)LoadImageW(NULL, 
                                          L"E:/test.bmp",  // 宽字符串
                                          IMAGE_BITMAP, 
                                          0, 0, 
                                          LR_LOADFROMFILE);
            
            if (g_hBitmap == NULL)
            {
                MessageBoxW(hwnd, L"Cannot load image file!\nPlease make sure D:/github/SoftRender/test.bmp exists.", L"Error", MB_OK | MB_ICONERROR);
                return -1;
            }
            
            // 其他代码保持不变...
            GetObject(g_hBitmap, sizeof(BITMAP), &g_bitmap);
            
            RECT rect = {0, 0, g_bitmap.bmWidth + 50, g_bitmap.bmHeight + 80};
            AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
            SetWindowPos(hwnd, NULL, 0, 0, 
                        rect.right - rect.left, 
                        rect.bottom - rect.top, 
                        SWP_NOMOVE | SWP_NOZORDER);
        }
        break;

    // ... 其他case保持不变 ...

    case WM_PAINT:
        {
            // 绘制代码与方案1相同
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (g_hBitmap != NULL)
            {
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int clientWidth = clientRect.right - clientRect.left;
                int clientHeight = clientRect.bottom - clientRect.top;
                
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ hOldBitmap = SelectObject(hdcMem, g_hBitmap);
                
                int destX, destY, destWidth, destHeight;
                
                if (g_bFitToWindow && (g_bitmap.bmWidth > clientWidth || g_bitmap.bmHeight > clientHeight))
                {
                    double scaleX = (double)clientWidth / g_bitmap.bmWidth;
                    double scaleY = (double)clientHeight / g_bitmap.bmHeight;
                    double scale = (scaleX < scaleY) ? scaleX : scaleY;
                    
                    destWidth = (int)(g_bitmap.bmWidth * scale);
                    destHeight = (int)(g_bitmap.bmHeight * scale);
                    destX = (clientWidth - destWidth) / 2;
                    destY = (clientHeight - destHeight) / 2;
                    
                    StretchBlt(hdc, destX, destY, destWidth, destHeight,
                              hdcMem, 0, 0, g_bitmap.bmWidth, g_bitmap.bmHeight, SRCCOPY);
                }
                else
                {
                    destX = (clientWidth - g_bitmap.bmWidth) / 2;
                    destY = (clientHeight - g_bitmap.bmHeight) / 2;
                    BitBlt(hdc, destX, destY, g_bitmap.bmWidth, g_bitmap.bmHeight,
                           hdcMem, 0, 0, SRCCOPY);
                }
                
                SelectObject(hdcMem, hOldBitmap);
                DeleteDC(hdcMem);
            }
            
            EndPaint(hwnd, &ps);
        }
        break;

    case WM_KEYDOWN:
        if (wParam == VK_SPACE)
        {
            g_bFitToWindow = !g_bFitToWindow;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        if (g_hBitmap != NULL)
        {
            DeleteObject(g_hBitmap);
            g_hBitmap = NULL;
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"BMPViewerWindow";
    
    WNDCLASSW wc = {};  // 使用 WNDCLASSW
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    
    RegisterClassW(&wc);  // 使用 RegisterClassW
    
    HWND hwnd = CreateWindowExW(  // 使用 CreateWindowExW
        0,
        CLASS_NAME,
        L"BMP Image Viewer - Press SPACE to toggle scaling",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL)
    {
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}