#include "CClient.h"

void CClient::SetUse(bool b)
{
	in_use = b;
}

void CClient::MoveNotify(int objID)
{
	this->lua_l.lock();
	lua_getglobal(this->L, "event_player_move");
	lua_pushnumber(this->L, objID);
	lua_pcall(this->L, 1, 1, 0);
	this->lua_l.unlock();
}

void CClient::Init(short x, short y, short level, char* name, int i)
{
	this->x = x;
	this->y = y;
	this->level = level;
	this->hp = level * 100;
	strcpy_s(this->name, name);

	this->is_active = false;
	this->L = luaL_newstate();

	int error = luaL_loadfile(L, "monster.lua");
	error = lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "set_uid");
	lua_pushnumber(L, i);
	lua_pcall(L, 1, 1, 0);
	// lua_pop(L, 1);// eliminate set_uid from stack after call

	lua_register(L, "API_SendEnterMessage", API_SendEnterMessage);
	lua_register(L, "API_SendLeaveMessage", API_SendLeaveMessage);
	lua_register(L, "API_get_x", API_get_x);
	lua_register(L, "API_get_y", API_get_y);
}

short CClient::getHP() const
{
	return this->hp;
}

bool CClient::getUse() const
{
	return this->in_use;
}
