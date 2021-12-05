#pragma once
#include"CCamera.h"
class CScene
{
	CCamera* camera;
	std::vector<CObject*> objects;
public:
	CScene();
	virtual ~CScene();

	virtual void Render(HDC hDC);
};