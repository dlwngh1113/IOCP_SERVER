#pragma once
#include"CObject.h"
class CTile: public CObject
{
public:
	CTile();
	CTile(LPCTSTR fileName, int x, int y);
	virtual ~CTile();
	virtual void Render(HDC MemDC, int scrollX, int scrollY) override;
	virtual void Move(int dx, int dy) override;
	virtual void Teleport(int x, int y) override;
	virtual void Update(float deltaTime) override;
};