#pragma once
#include"../../client/2021_Client/Client/CMapLoader.h"
class CMap
{
	short map[WORLD_WIDTH][WORLD_HEIGHT]{ 0 };
public:
	CMap() = default;
	CMap(const char* fileName);
	virtual ~CMap();
	void ProcessMove(int id, int dir);
	void RandomMove(int id);
};

