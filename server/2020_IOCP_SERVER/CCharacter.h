#pragma once
#include"framework.h"

class CCharacter
{
	std::string name;
	short x{ 0 }, y{ 0 };
public:
	CCharacter() = default;
	CCharacter(std::string name, short x, short y);
	virtual ~CCharacter() = 0;

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);

	virtual std::string& GetName();
	virtual short& GetX();
	virtual short& GetY();
};