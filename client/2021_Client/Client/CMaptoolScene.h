#pragma once
#include "CScene.h"
#include "CTile.h"
class CMaptoolScene : public CScene
{
	short map[800][800];
public:
	CMaptoolScene();
	virtual ~CMaptoolScene();
	virtual void SethWnd(HWND hWnd) { CScene::SethWnd(hWnd); }
	virtual void Render(HDC hDC) override;
	virtual LRESULT KeyInputProcess(WPARAM wParam, LPARAM lParam)override;
	virtual LRESULT MouseInputProcess(UINT message, WPARAM wParam, LPARAM lParam)override;
};

