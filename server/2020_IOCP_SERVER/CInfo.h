#pragma once
#include"framework.h"
class CInfo
{
public:
	int id{ 0 };
	std::string name;
	short x{ 0 }, y{ 0 };

	bool isAttkable{ true };
	short atk{ 0 };
	short hp{ 0 };
	short level{ 0 };
	int exp;

	int atk_time{ 0 };
	int move_time{ 0 };

	std::mutex c_lock;

	CInfo() = default;
	CInfo(int id, std::string name, short x, short y);
	virtual ~CInfo();
};

