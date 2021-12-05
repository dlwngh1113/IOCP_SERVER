#include "pch.h"
#include "CCamera.h"

CCamera::~CCamera()
{
}

void CCamera::Render(HDC MemDC)
{
}

void CCamera::Render(HDC MemDC, const std::vector<CObject*>& objects)
{
	for (auto& obj : objects)
		obj->Render(MemDC, scrollX, scrollY);
}
