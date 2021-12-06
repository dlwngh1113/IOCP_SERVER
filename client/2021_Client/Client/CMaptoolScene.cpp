#include "pch.h"
#include "CMaptoolScene.h"

CMaptoolScene::CMaptoolScene() : CScene()
{
}

CMaptoolScene::~CMaptoolScene()
{
	printf("maptool scene destructor called");
	delete camera;
	for (auto& obj : objects)
		delete obj;
}

void CMaptoolScene::Render(HDC hDC)
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

LRESULT CMaptoolScene::KeyInputProcess(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		camera->Move(-32, 0);
		break;
	case VK_RIGHT:
		camera->Move(32, 0);
		break;
	case VK_UP:
		camera->Move(0, -32);
		break;
	case VK_DOWN:
		camera->Move(0, 32);
		break;
	}
	return 0;
}

LRESULT CMaptoolScene::MouseInputProcess(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		int nx = LOWORD(lParam) / 32 * 32;
		int ny = HIWORD(lParam) / 32 * 32;
		objects.emplace_back(new CTile(L"../../2020_CLIENT/Resources/ghost.png", camera->GetScroll().x + nx, camera->GetScroll().y + ny));
	}
	break;
	default:
		break;
	}
	return 0;
}
