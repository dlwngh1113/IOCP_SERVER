#include "CMapLoader.h"

void CMapLoader::LoadMap(short map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName)
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

void CMapLoader::SaveMap(short map[WORLD_WIDTH][WORLD_HEIGHT], const char* fileName)
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
