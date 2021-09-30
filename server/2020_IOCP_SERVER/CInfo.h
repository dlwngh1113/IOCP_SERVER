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

	CInfo() = default;
	CInfo(int id, std::string name, short x, short y);
	virtual ~CInfo();
};

