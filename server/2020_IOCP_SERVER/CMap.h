#pragma once
#include"CMapLoader.h"
#include"CClient.h"
#include"CMonster.h"
class CMap
{
	short map[WORLD_WIDTH][WORLD_HEIGHT]{ 0 };
public:
	CMap() = default;
	CMap(const char* fileName);
	virtual ~CMap();
	void GetValidPosition(int& x, int& y);
	void ProcessMove(CClient* client, int dir);
	void RandomMove(CMonster* npc);
};

