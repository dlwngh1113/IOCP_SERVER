#pragma once
#include"framework.h"

class CCharacter
{
	int id{ 0 };
	std::string name;
	short x{ 0 }, y{ 0 };
	std::unordered_set<int> viewList;
	std::mutex viewLock;
public:
	CCharacter() = default;
	CCharacter(int id, std::string name, short x, short y);
	virtual ~CCharacter() = 0;

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);

	virtual std::unordered_set<int>& GetViewlist();
	virtual std::mutex& GetViewlock();
	virtual std::string& GetName();
	virtual short GetX() const;
	virtual short GetY() const;
	virtual int GetID() const;
};