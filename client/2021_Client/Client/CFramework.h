#pragma once
#include"CMaptoolScene.h"

class CFramework
{
    CScene* scene;
public:
    CFramework();
    virtual ~CFramework();
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};