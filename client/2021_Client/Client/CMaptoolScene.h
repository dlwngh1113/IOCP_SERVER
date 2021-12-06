#pragma once
#include "CScene.h"
#include "CTile.h"
class CMaptoolScene : public CScene
{
	short map[WORLD_WIDTH][WORLD_HEIGHT]{ NULL };
	std::vector<CImage*> tiles;
	int curTile{ 0 };
public:
	CMaptoolScene();
	virtual ~CMaptoolScene();
	virtual void SethWnd(HWND hWnd) { CScene::SethWnd(hWnd); }
	virtual void Render(HDC hDC) override;
	virtual LRESULT KeyInputProcess(WPARAM wParam, LPARAM lParam)override;
	virtual LRESULT MouseInputProcess(UINT message, WPARAM wParam, LPARAM lParam)override;
};

