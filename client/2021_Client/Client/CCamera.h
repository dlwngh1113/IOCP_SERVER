#pragma once
#include"CObject.h"
class CCamera
{
	POINT scroll{ NULL };
public:
	CCamera() = default;
	virtual ~CCamera();

	POINT& GetScroll() { return scroll; }

	void RenderSingleImage(HDC MemDC, CImage img, int x, int y, int w, int h) const;
	void Render(HDC MemDC, const std::vector<CObject*>& objects) const;
	void Render(HDC MemDC, const CObject* object) const;
	void Move(int dx, int dy);
};