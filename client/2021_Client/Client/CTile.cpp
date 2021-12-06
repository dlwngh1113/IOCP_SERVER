#include "pch.h"
#include "CTile.h"

CTile::CTile()
{
}

CTile::CTile(LPCTSTR fileName, int x, int y) :CObject(POINT{ x, y })
{
	image.Load(fileName);
}

CTile::~CTile()
{
}

void CTile::Render(HDC MemDC, int scrollX, int scrollY) const
{
	image.StretchBlt(MemDC, position.x - scrollX, position.y - scrollY, TILE_SIZE, TILE_SIZE);
}

void CTile::Move(int dx, int dy)
{
	position.x += TILE_SIZE;
	position.y += TILE_SIZE;
}

void CTile::Teleport(int x, int y)
{
	position.x = x;
	position.y = y;
}

void CTile::Update(float deltaTime)
{
}
