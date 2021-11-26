#pragma once

class CFramework
{
    HINSTANCE hInst;
public:
    CFramework() = default;
    CFramework(HINSTANCE hInst);
    virtual ~CFramework() {}
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

