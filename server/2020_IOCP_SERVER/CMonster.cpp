#include "CMonster.h"

CMonster::CMonster() :CCharacter()
{
}

CMonster::CMonster(int id, std::string name, short x, short y, short level) : CCharacter(id, name, x, y)
{
	this->level = level;
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

std::unordered_set<int>& CMonster::GetViewlist()
{
	return CCharacter::GetViewlist();
}

short CMonster::GetX()
{
	return CCharacter::GetX();
}

short CMonster::GetY()
{
	return CCharacter::GetY();
}
