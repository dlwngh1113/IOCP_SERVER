#include "pch.h"
#include "CScene.h"

CScene::CScene()
{
	camera = new CCamera;
	objects = std::vector<CObject*>();
}

CScene::~CScene()
{
	delete camera;
	for (auto obj : objects)
		delete obj;
	objects.clear();
}

void CScene::Render(HDC hDC)
{
	HDC MemDC = CreateCompatibleDC(hDC);
	HBITMAP hBit = CreateCompatibleBitmap(hDC, SCREEN_WIDTH, SCREEN_HEIGHT);
	HBITMAP oldBit = (HBITMAP)SelectObject(MemDC, hBit);

	PatBlt(MemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITENESS);

	//Add Rendering Code
	camera->Render(MemDC, objects);
	//Render Code End

	BitBlt(hDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, oldBit);
	DeleteObject(hBit);
	DeleteDC(MemDC);
}

LRESULT CScene::KeyInputProcess(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		break;
	case VK_RIGHT:
		break;
	case VK_UP:
		break;
	case VK_DOWN:
		break;
	}
	return 0;
}

LRESULT CScene::MouseInputProcess(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	default:
		break;
	}
	return 0;
}
