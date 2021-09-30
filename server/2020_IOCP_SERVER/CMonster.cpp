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

std::mutex& CMonster::GetViewlock()
{
	return CCharacter::GetViewlock();
}

std::unordered_set<int>& CMonster::GetViewlist()
{
	return CCharacter::GetViewlist();
}

void CMonster::MoveNotify(int id)
{
	lua_l.lock();
	lua_getglobal(L, "event_player_move");
	lua_pushnumber(L, id);
	lua_pcall(L, 1, 1, 0);
	lua_l.unlock();
}
