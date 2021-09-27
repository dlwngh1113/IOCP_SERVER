#include "CMonster.h"

CMonster::CMonster() :CCharacter()
{
}

CMonster::CMonster(std::string name, short x, short y) : CCharacter(name, x, y)
{
}

CMonster::~CMonster()
{
}

void CMonster::Move(short dx, short dy)
{
	CCharacter::Move(dx, dy);
}

void CMonster::Teleport(short x, short y)
{
	CCharacter::Teleport(x, y);
}

std::string& CMonster::GetName()
{
	return CCharacter::GetName();
}

short& CMonster::GetX()
{
	return CCharacter::GetX();
}

short& CMonster::GetY()
{
	return CCharacter::GetY();
}
