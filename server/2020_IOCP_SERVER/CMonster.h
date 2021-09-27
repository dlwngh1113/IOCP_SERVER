#pragma once
#include"CCharacter.h"
class CMonster : CCharacter
{
public:
	CMonster();
	virtual ~CMonster();

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);
};

