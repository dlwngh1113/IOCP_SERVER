#pragma once
#include"CInfo.h"

class CCharacter
{
	CInfo* info;
	
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
};