#include "pch.h"
#include "CMapLoader.h"

template<class T>
void CMapLoader::LoadMap(T map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName)
{
	std::ifstream in(fileName);
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

template<class T>
void CMapLoader::SaveMap(T map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName)
{
	std::ofstream out(fileName);

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
