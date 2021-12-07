#include "pch.h"
#include "CMaptoolScene.h"

CMaptoolScene::CMaptoolScene() : CScene()
{
	CImage* tmp = new CImage;
	tmp->Load(L"../../2020_CLIENT/Resources/tile1.png");
	tiles.emplace_back(tmp);
	tmp = new CImage;
	tmp->Load(L"../../2020_CLIENT/Resources/tile2.png");
	tiles.emplace_back(tmp);
	tmp = new CImage;
	tmp->Load(L"../../2020_CLIENT/Resources/tile3.png");
	tiles.emplace_back(tmp);

	GenerateRandomMap();
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
	int startX = camera->GetScroll().x / TILE_SIZE;
	int startY = camera->GetScroll().y / TILE_SIZE;
	
	int x = 0;
	int y = 0;
	for (int i = startX; i < startX + SCREEN_WIDTH / TILE_SIZE; ++i) {
		x = std::clamp(i, 0, WORLD_WIDTH - 1);
		for (int j = startY; j < startY + SCREEN_HEIGHT / TILE_SIZE; ++j)
		{
			y = std::clamp(j, 0, WORLD_HEIGHT - 1);
			tiles[map[x][y]]->StretchBlt(MemDC, x * TILE_SIZE - camera->GetScroll().x, y * TILE_SIZE - camera->GetScroll().y, TILE_SIZE, TILE_SIZE);
		}
	}
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
	case '2':
		curTile = 2;
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
		map[std::clamp(i, 0, WORLD_WIDTH - 1)][std::clamp(j, 0, WORLD_HEIGHT - 1)] = curTile;
	}
	break;
	default:
		break;
	}
	return 0;
}

void CMaptoolScene::GenerateRandomMap()
{
	for (int i = 0; i < WORLD_WIDTH; ++i)
	{
		for (int j = 0; j < WORLD_HEIGHT; ++j)
		{
			switch (rand() % 10)
			{
			case 0:
				map[i][j] = 1;
				break;
			case 1:
				map[i][j] = 2;
				break;
			default:
				map[i][j] = 0;
				break;
			}
		}
	}
	map[0][0] = 0;
}
