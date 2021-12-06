#pragma once
#include"CObject.h"
constexpr int TILE_SIZE = 32;
constexpr int WORLD_WIDTH = 800;
constexpr int WORLD_HEIGHT = 800;

class CTile: public CObject
{
public:
	CTile();
	CTile(LPCTSTR fileName, int x, int y);
	virtual ~CTile();
	virtual void Render(HDC MemDC, int scrollX, int scrollY) const override;
	virtual void Move(int dx, int dy) override;
	virtual void Teleport(int x, int y) override;
	virtual void Update(float deltaTime) override;
};