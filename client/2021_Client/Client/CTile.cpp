#include "pch.h"
#include "CTile.h"

CTile::CTile()
{
}

CTile::~CTile()
{
}

void CTile::Render(HDC MemDC, int scrollX, int scrollY)
{
	image.StretchBlt(MemDC, position);
}

void CTile::Move(int dx, int dy)
{
	position.left += dx;
	position.right += dx;
	position.bottom += dy;
	position.top += dy;
}

void CTile::Teleport(int x, int y)
{
	short hWidth = (position.right - position.left) / 2;
	short hHeight = (position.bottom - position.top) / 2;

	position.left = x - hWidth;
	position.right = x + hWidth;
	position.top = y - hHeight;
	position.bottom = y + hHeight;
}

void CTile::Update(float deltaTime)
{
}
