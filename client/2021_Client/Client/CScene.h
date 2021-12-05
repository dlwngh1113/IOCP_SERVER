#pragma once
#include"CCamera.h"
class CScene
{
protected:
	HWND hWnd;
	CCamera* camera;
	std::vector<CObject*> objects;
public:
	CScene();
	virtual ~CScene();

	virtual void SethWnd(HWND hWnd) { this->hWnd = hWnd; }
	virtual void Render(HDC hDC);
	virtual LRESULT KeyInputProcess(WPARAM wParam, LPARAM lParam);
	virtual LRESULT MouseInputProcess(UINT message, WPARAM wParam, LPARAM lParam);
};