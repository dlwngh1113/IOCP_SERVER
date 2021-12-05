#include "pch.h"
#include "CScene.h"

CScene::CScene()
{
	camera = new CCamera;
}

CScene::~CScene()
{
	delete camera;
}

void CScene::Render(HDC hDC)
{
}
