#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include"..\..\server\2020_IOCP_SERVER\protocol.h"
#include <windows.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include<fstream>
class CMapLoader
{
public:
	static void LoadMap(short map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName);
	static void SaveMap(short map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName);
};

