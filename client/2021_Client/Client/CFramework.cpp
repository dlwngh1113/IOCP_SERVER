#include "pch.h"
#include "CFramework.h"

CFramework::CFramework(HINSTANCE hInst) : hInst{ hInst }
{
    scene = new CScene;
}

LRESULT CALLBACK CFramework::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
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
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}