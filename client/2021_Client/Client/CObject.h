#pragma once
class CObject
{
protected:
	RECT position{ NULL };
	CImage image;
public:
	CObject() = default;
	virtual ~CObject() {};
	virtual void Move(int dx, int dy) = 0;
	virtual void Teleport(int x, int y) = 0;
	virtual void Render(HDC MemDC, int scrollX, int scrollY) = 0;
	virtual void Update(float deltaTime) = 0;
};