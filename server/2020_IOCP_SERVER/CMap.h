#pragma once
#include"framework.h"

class CMap
{
	short foreGround[WORLD_WIDTH * WORLD_HEIGHT];
	short backGround[WORLD_WIDTH * WORLD_HEIGHT];
public:
	CMap() = default;
	virtual ~CMap();

	bool LoadMapFile(const char* fileName);
};