// WindowsProject4.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject4.h"
#include "math.h"
#include "timeapi.h"

#define MAX_LOADSTRING 100
HWND hWnd;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct Blocks {
    int xcenter;
    int ycenter;
    int width;
    int height;
    bool show;
};

struct Ball {
    int xcenter;
    int ycenter;
    int R;
    bool show;
};

struct Racket {
    int x;
    int y;
    int width;
    int height;
    int speed = 1;
};



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT4));

    

    HDC device_context = GetDC(hWnd);
    RECT r;
    GetClientRect(hWnd, &r);
    auto width = r.right - r.left;//определяем размеры и сохраняем
    auto height = r.bottom - r.top;

    HDC context = CreateCompatibleDC(device_context);

    SelectObject(context, CreateCompatibleBitmap(device_context, width, height));//привязываем окно к контексту
    
    GetClientRect(hWnd, &r);

    MSG msg = { 0 };

    
    const int xcount = 7;
    const int ycount = 16;
    const int blsize = xcount*ycount;
    const int border = 2;
    const float yinterval = 0.06;
    const float blocksheightcoef = 0.2;

    Blocks blocks[blsize];

    for (int i = 0; i < blsize; i++) {
        blocks[i].xcenter = i % xcount * width / xcount + width / (2*xcount);
        blocks[i].ycenter = i / xcount * width / (2 * xcount) + width * yinterval;
        blocks[i].width = width / xcount - border;
        blocks[i].height = width * yinterval;

        blocks[i].ycenter *= blocksheightcoef;
        blocks[i].height *= blocksheightcoef;

        blocks[i].show = 1;
    }

    Racket racket;
    racket.width = width / xcount - border;
    racket.height = 1.5 * width * yinterval * blocksheightcoef;
    racket.x = width / 2;
    racket.y = height - racket.height - border;



    // Main message loop:
    while (msg.message != WM_QUIT)
    {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);

            }

            if (GetAsyncKeyState(VK_LEFT) && racket.x > width / (2 * xcount) + 2 * border) racket.x -= racket.speed;
            if (GetAsyncKeyState(VK_RIGHT) && racket.x < width - width / (2 * xcount) - 2 * border) racket.x += racket.speed;
          
            

            HBRUSH brush;

            brush = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 255, 0)); // Создаём кисть определённого стиля и цвета

            RECT WinCoord = {}; //Массив координат окна 
            GetWindowRect(hWnd, &WinCoord); //Узнаём координаты

            brush = CreateSolidBrush(RGB(0, 0, 0));
            SelectObject(context, brush); //Выбираем кисть
            Rectangle(context, 0, 0, WinCoord.right, WinCoord.bottom); //фон
            DeleteObject(brush);

            brush = CreateSolidBrush(RGB(0, 0, 255)); //Создаём кисть определённого стиля и цвета
            SelectObject(context, brush); //Выбираем кисть

            for (int i = 0; i < blsize; i++) {
                if (blocks[i].show) {
                    Rectangle(context, blocks[i].xcenter - blocks[i].width/2, blocks[i].ycenter - blocks[i].height / 2, blocks[i].xcenter + blocks[i].width / 2, blocks[i].ycenter + blocks[i].height / 2);
                }
            }
            
            //Rectangle(context, WinCoord.right/3, WinCoord.bottom / 3, WinCoord.right / 3 + 100 * cos(timeGetTime() * .001), WinCoord.bottom / 3 + 100 * sin(timeGetTime() * .001)); //Рисуем новый прямоугольник, который будет небом
            DeleteObject(brush); //Очищаем память от созданной, но уже ненужной кисти

            brush = CreateSolidBrush(RGB(0, 255, 255));
            SelectObject(context, brush); //Выбираем кисть
            Rectangle(context, racket.x - racket.width / 2, racket.y - racket.height / 2, racket.x + racket.width / 2, racket.y + racket.height / 2);
            DeleteObject(brush);

            BitBlt(device_context, 0, 0, WinCoord.right, WinCoord.bottom, context, 0, 0, SRCCOPY);

            // TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);

     }

    ExitProcess(0);


    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
