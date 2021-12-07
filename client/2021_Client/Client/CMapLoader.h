#pragma once
#include"CTile.h"
class CMapLoader
{
public:
	static void LoadMap(short map[WORLD_WIDTH][WORLD_HEIGHT]);
	static void SaveMap(short map[WORLD_WIDTH][WORLD_HEIGHT]);
};

