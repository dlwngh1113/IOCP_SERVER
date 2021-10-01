#pragma once
#include"CInfo.h"

class CCharacter
{
	CInfo* info{ nullptr };
	
protected:
	std::unordered_set<int> viewList;
	std::mutex viewLock;

	std::mutex c_lock;
public:
	CCharacter() = default;
	CCharacter(int id, std::string name, short x, short y);
	CCharacter(CInfo* info);
	virtual ~CCharacter();

	virtual void Move(short dx, short dy);
	virtual void Teleport(short x, short y);
	virtual bool GetDamage(short otherAtk);

	CInfo* GetInfo();
};