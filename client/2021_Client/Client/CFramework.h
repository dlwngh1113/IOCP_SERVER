#pragma once
#include"CScene.h"

class CFramework
{
    HINSTANCE hInst;
    CScene* scene;
public:
    CFramework() = default;
    CFramework(HINSTANCE hInst);
    virtual ~CFramework() {}
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

