#include "CMonster.h"

CMonster::CMonster() :CCharacter()
{
	L = luaL_newstate();
}

CMonster::CMonster(int id, std::string name, short x, short y, short level) : CCharacter(id, name, x, y)
{
	L = luaL_newstate();
	luaL_openlibs(L);

	int error = luaL_loadfile(L, "monster.lua");
	error = lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "set_uid");
	lua_pushnumber(L, id);
	lua_pcall(L, 1, 1, 0);
	lua_pop(L, 1);// eliminate set_uid from stack after call
}

CMonster::CMonster(CInfo* info) : CCharacter{ info }
{
	L = luaL_newstate();
	luaL_openlibs(L);

	int error = luaL_loadfile(L, "monster.lua");
	error = lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "set_uid");
	lua_pushnumber(L, info->id);
	lua_pcall(L, 1, 1, 0);
	lua_pop(L, 1);// eliminate set_uid from stack after call
}

CMonster::~CMonster()
{
}

lua_State* CMonster::GetLua()
{
	return L;
}

void CMonster::Move(short dx, short dy)
{
	CCharacter::Move(dx, dy);
}

void CMonster::Teleport(short x, short y)
{
	CCharacter::Teleport(x, y);
}

void CMonster::MoveNotify(int id)
{
	lua_l.lock();
	lua_getglobal(L, "event_player_move");
	lua_pushnumber(L, id);
	lua_pcall(L, 1, 1, 0);
	lua_l.unlock();
}
