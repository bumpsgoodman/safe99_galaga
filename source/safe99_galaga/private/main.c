//***************************************************************************
// 
// 파일: main.c
// 
// 설명: main 함수
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/17
// 
//***************************************************************************

#include "precompiled.h"
#include "game.h"

HINSTANCE g_hinstance;
HWND g_hwnd;

bool init_window(const HINSTANCE h_instance, const size_t width, const size_t height);
LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE h_prev_instance, _In_ LPWSTR lp_cmd_line, _In_ int cmd_show)
{
    const size_t window_width = 800;
    const size_t window_height = 600;

    // 윈도우 생성
    if (!init_window(hinstance, window_width, window_height))
    {
        ASSERT(false, "Failed to init window");
        goto failed_init_window;
    }

    // 게임 객체 생성/초기화
    gp_game = (game_t*)malloc(sizeof(game_t));
    ASSERT(gp_game != NULL, "gp_game == NULL");
    if (!init_game())
    {
        ASSERT(false, "Failed to init game");
        goto failed_init_game;
    }

    // 프레임 타이머 초기화
    timer_t frame_timer;
    timer_init(&frame_timer);
    timer_start(&frame_timer);

    float start_time = timer_get_time(&frame_timer);

    // 메시지 루프
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            const float end_time = timer_get_time(&frame_timer);
            const float delta_time = end_time - start_time;
            if (delta_time >= get_limit_frame_rate())
            {
                if (!is_running_game())
                {
                    break;
                }

                tick_game();

                start_time = end_time;
            }
        }
    }

    // 게임 종료
    shutdown_game();
    SAFE_FREE(gp_game);

    return (int)msg.wParam;

failed_init_game:
    SAFE_FREE(gp_game);

failed_init_window:
    return -1;
}

bool init_window(const HINSTANCE hinstance, const size_t width, const size_t height)
{
    // 클래스 등록
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hinstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"safe99 - galaga";
    wcex.hIconSm = NULL;
    if (!RegisterClassEx(&wcex))
    {
        return false;
    }

    // 윈도우 생성
    g_hinstance = hinstance;
    RECT rc = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hwnd = CreateWindow(wcex.lpszClassName, L"safe99 - galaga",
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, wcex.hInstance,
                          NULL);
    if (!g_hwnd)
    {
        return false;
    }

    ShowWindow(g_hwnd, SW_SHOWDEFAULT);

    return true;
}

LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        on_key_down((uint8_t)wParam);
        break;
    case WM_KEYUP:
        on_key_up((uint8_t)wParam);
        break;

    case WM_MOVE:
        if (gp_game != NULL)
        {
            update_window_pos_game();
        }
        break;
    case WM_SIZE:
        if (gp_game != NULL)
        {
            update_window_size_game();
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}