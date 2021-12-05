#pragma once
#include"CCamera.h"
class CScene
{
	CCamera* camera;
public:
	CScene();
	virtual ~CScene();

	virtual void Render(HDC hDC);
};

