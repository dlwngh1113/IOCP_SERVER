#include "pch.h"
#include "CMapLoader.h"

void CMapLoader::LoadMap(short map[WORLD_WIDTH][WORLD_HEIGHT])
{
	std::ifstream in("map.txt");
	int w, h;

	in >> w >> h;
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			in >> map[i][j];
		}
	}
}

void CMapLoader::SaveMap(short map[WORLD_WIDTH][WORLD_HEIGHT])
{
	std::ofstream out("map.txt");

	out << WORLD_HEIGHT << ' ' << WORLD_WIDTH << '\n';

	for (int i = 0; i < WORLD_WIDTH; ++i)
	{
		for (int j = 0; j < WORLD_HEIGHT; ++j)
		{
			out << map[i][j] << ' ';
		}
		out << '\n';
	}

	out.close();
}
