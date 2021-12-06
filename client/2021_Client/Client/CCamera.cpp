#include "pch.h"
#include "CCamera.h"

CCamera::~CCamera()
{
}

void CCamera::Render(HDC MemDC) const
{
}

void CCamera::Render(HDC MemDC, const std::vector<CObject*>& objects) const
{
	for (auto& obj : objects)
		obj->Render(MemDC, scroll.x, scroll.y);
}

void CCamera::Render(HDC MemDC, const CObject* object) const
{
	object->Render(MemDC, scroll.x, scroll.y);
}

void CCamera::Move(int dx, int dy)
{
	scroll.x += dx;
	scroll.y += dy;
}
