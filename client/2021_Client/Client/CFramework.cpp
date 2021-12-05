#include "pch.h"
#include "CFramework.h"

CFramework::CFramework()
{
    scene = new CMaptoolScene;
}

CFramework::~CFramework()
{
    delete scene;
}

LRESULT CFramework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        scene->SethWnd(hWnd);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        scene->Render(hdc);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        delete this;
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
    case WM_CHAR:
        scene->KeyInputProcess(wParam, lParam);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
        scene->MouseInputProcess(message, wParam, lParam);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}