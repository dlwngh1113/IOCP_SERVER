#pragma once
#include"CObject.h"
class CCamera
{
	POINT scroll;
public:
	CCamera() = default;
	virtual ~CCamera();

	POINT& GetScroll() { return scroll; }

	void Render(HDC MemDC) const;
	void Render(HDC MemDC, const std::vector<CObject*>& objects) const;
	void Render(HDC MemDC, const CObject* object) const;
	void Move(int dx, int dy);
};