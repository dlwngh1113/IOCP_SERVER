#include "pch.h"
#include "CMaptoolScene.h"

CMaptoolScene::CMaptoolScene() : CScene()
{
	CImage* tmp = new CImage;
	tmp->Load(L"../../2020_CLIENT/Resources/ghost.png");
	tiles.emplace_back(tmp);
	tmp = new CImage;
	tmp->Load(L"../../2020_CLIENT/Resources/player.png");
	tiles.emplace_back(tmp);
}

CMaptoolScene::~CMaptoolScene()
{
	delete camera;
	for (auto& obj : objects)
		delete obj;
	for (auto& img : tiles)
		delete img;
	tiles.clear();
	objects.clear();
}

void CMaptoolScene::Render(HDC hDC)
{
	HDC MemDC = CreateCompatibleDC(hDC);
	HBITMAP hBit = CreateCompatibleBitmap(hDC, SCREEN_WIDTH, SCREEN_HEIGHT);
	HBITMAP oldBit = (HBITMAP)SelectObject(MemDC, hBit);

	PatBlt(MemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITENESS);

	//Add Rendering Code
	POINT x{ camera->GetScroll().x / TILE_SIZE, (camera->GetScroll().x + SCREEN_WIDTH) / TILE_SIZE };
	POINT y{ camera->GetScroll().y / TILE_SIZE, (camera->GetScroll().y + SCREEN_HEIGHT) / TILE_SIZE };
	for (int i = x.x; i < x.y; ++i)
		for (int j = y.x; j < y.y; ++j)
			tiles[map[std::clamp(i, 0, WORLD_WIDTH)][std::clamp(j, 0, WORLD_HEIGHT)]]->
			StretchBlt(MemDC, i * TILE_SIZE - camera->GetScroll().x, j * TILE_SIZE - camera->GetScroll().y, TILE_SIZE, TILE_SIZE);
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
		camera->Move(-TILE_SIZE, 0);
		break;
	case VK_RIGHT:
		camera->Move(TILE_SIZE, 0);
		break;
	case VK_UP:
		camera->Move(0, -TILE_SIZE);
		break;
	case VK_DOWN:
		camera->Move(0, TILE_SIZE);
		break;
	case '0':
		curTile = 0;
		break;
	case '1':
		curTile = 1;
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
		int nx = LOWORD(lParam);
		int ny = HIWORD(lParam);
		int i = (nx + camera->GetScroll().x) / TILE_SIZE;
		int j = (ny + camera->GetScroll().y) / TILE_SIZE;
		map[std::clamp(i, 0, WORLD_WIDTH)][std::clamp(j, 0, WORLD_HEIGHT)] = curTile;
	}
	break;
	default:
		break;
	}
	return 0;
}
