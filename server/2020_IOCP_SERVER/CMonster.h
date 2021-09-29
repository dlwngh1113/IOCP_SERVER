#pragma once
#include"CCharacter.h"
class CMonster : public CCharacter
{
	short level{ 0 };
	lua_State* L;
	std::mutex lua_l;
public:
	CMonster();
	CMonster(int id, std::string name, short x, short y, short level);
	virtual ~CMonster();

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);

	std::string& GetName();
	std::unordered_set<int>& GetViewlist();
	short GetX();
	short GetY();
};

