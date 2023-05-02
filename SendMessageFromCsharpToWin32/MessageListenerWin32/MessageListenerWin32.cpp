#include "framework.h"
#include <string>
#include "MessageListenerWin32.h"

#define MAX_LOADSTRING 100
constexpr auto WM_CUSTOM1 = WM_APP + 0;
constexpr auto WM_CUSTOM2 = WM_APP + 1;
constexpr auto EventName1 = L"MessageListenerWin32_EventName1";
constexpr auto EventName2 = L"MessageListenerWin32_EventName2";
constexpr auto Title = L"Message Listener Win32";
constexpr auto WindowClassName = L"MessageListenerWin32_WindowClassName";

struct  MyThreadArg
{
    HWND hWnd;
    HANDLE EventHandle;
    HDC hdc;
    bool IsContinue;
};

HINSTANCE hInst;                                // 現在のインターフェイス

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI ThreadFunc1(LPVOID);
DWORD WINAPI ThreadFunc2(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MESSAGELISTENERWIN32));

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MESSAGELISTENERWIN32));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MESSAGELISTENERWIN32);
    wcex.lpszClassName = WindowClassName;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindowW(WindowClassName, Title, WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static MyThreadArg thread1Arg, thread2Arg;
    static HANDLE thread1, thread2;
    static DWORD thread1Id, thread2Id;
    static HDC hdc;

    switch (message)
    {
        case WM_CREATE:
        {
            hdc = GetDC(hWnd);
            thread1Arg = {
                hWnd,
                CreateEventW(nullptr, FALSE, FALSE, EventName1),
                hdc,
                true,
            };
            thread2Arg = {
                hWnd,
                CreateEventW(nullptr, FALSE, FALSE, EventName2),
                hdc,
                true,
            };
            thread1 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadFunc1, &thread1Arg, 0, &thread1Id);
            thread2 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadFunc2, &thread2Arg, 0, &thread2Id);
            break;
        }
        case WM_CUSTOM1:
        {
            SetEvent(thread1Arg.EventHandle);
            break;
        }
        case WM_CUSTOM2:
        {
            SetEvent(thread2Arg.EventHandle);
            break;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
                case IDM_ABOUT:
                {
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                }
                case IDM_EXIT:
                {
                    DestroyWindow(hWnd);
                    break;
                }
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_DESTROY:
        {
            // スレッドを終了する
            thread1Arg.IsContinue = false;
            thread2Arg.IsContinue = false;
            SetEvent(thread1Arg.EventHandle);
            SetEvent(thread2Arg.EventHandle);
            WaitForSingleObject(thread1, INFINITE);
            WaitForSingleObject(thread2, INFINITE);

            // ハンドルを閉じる
            CloseHandle(thread1);
            CloseHandle(thread2);

            // リソース開放
            ReleaseDC(hWnd, hdc);

            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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

DWORD WINAPI ThreadFunc1(LPVOID threadArg)
{
    auto lpd = reinterpret_cast<MyThreadArg*>(threadArg);

    while (lpd->IsContinue)
    {
        WaitForSingleObject(lpd->EventHandle, INFINITE);
        if (!lpd->IsContinue)
            break;
        SetTextColor(lpd->hdc, RGB(255, 0, 0));
        auto str = std::wstring{ L"WM_CUSTOM1 Received." };
        TextOutW(lpd->hdc, 5, 5, str.c_str(), str.length());
    }

    return 0;
}

DWORD WINAPI ThreadFunc2(LPVOID threadArg)
{
    auto lpd = reinterpret_cast<MyThreadArg*>(threadArg);

    while (lpd->IsContinue)
    {
        WaitForSingleObject(lpd->EventHandle, INFINITE);
        if (!lpd->IsContinue)
            break;
        SetTextColor(lpd->hdc, RGB(0, 0, 255));
        auto str = std::wstring{ L"WM_CUSTOM2 Received." };
        TextOutW(lpd->hdc, 5, 5, str.c_str(), str.length());
    }

    return 0;
}
