#pragma once
class CObject
{
protected:
	POINT position{ NULL };
	CImage image;
public:
	CObject() = default;
	CObject(POINT p) :position{ p } {}
	virtual ~CObject() {};
	virtual void Move(int dx, int dy) = 0;
	virtual void Teleport(int x, int y) = 0;
	virtual void Render(HDC MemDC, int scrollX, int scrollY) const = 0;
	virtual void Update(float deltaTime) = 0;
};