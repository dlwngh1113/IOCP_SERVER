#pragma once
#include"CTile.h"
class CMapLoader
{
public:
	template<class T>
	static void LoadMap(T map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName);
	template<class T>
	static void SaveMap(T map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName);
};

