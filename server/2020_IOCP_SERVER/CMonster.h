#pragma once
#include"CCharacter.h"
class CMonster : public CCharacter
{
	lua_State* L;
	std::mutex lua_l;
public:
	CMonster();
	CMonster(int id, std::string name, short x, short y, short level);
	CMonster(CInfo* info);
	virtual ~CMonster();

	lua_State* GetLua();

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);

	void MoveNotify(int id);
};

