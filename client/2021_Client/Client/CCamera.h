#pragma once
#include"CObject.h"
class CCamera
{
	int scrollX{ 0 };
	int scrollY{ 0 };
public:
	CCamera() = default;
	virtual ~CCamera();

	void Render(HDC MemDC);
	void Render(HDC MemDC, const std::vector<CObject*>& objects);
};