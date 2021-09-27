#pragma once
#include"CCharacter.h"
class CMonster : CCharacter
{
public:
	CMonster();
	CMonster(std::string name, short x, short y);
	virtual ~CMonster();

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);

	std::string& GetName();
	short& GetX();
	short& GetY();
};

