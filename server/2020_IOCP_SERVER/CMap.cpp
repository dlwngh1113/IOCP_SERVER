#include "CMap.h"
#include "CServer.h"

CMap::CMap(const char* fileName)
{
	CMapLoader::LoadMap(map, fileName);
}

CMap::~CMap()
{
}

void CMap::GetValidPosition(int& x, int& y)
{
	int tmpX = rand() % WORLD_WIDTH;
	int tmpY = rand() % WORLD_HEIGHT;
	while (map[tmpX][tmpY] != 0)
	{
		tmpX = rand() % WORLD_WIDTH;
		tmpY = rand() % WORLD_HEIGHT;
	}
	x = tmpX;
	y = tmpY;
}

void CMap::ProcessMove(CClient* client, int dir)
{
	short x = client->GetInfo()->x;
	short y = client->GetInfo()->y;
	switch (dir) {
	case MV_UP: 
		if (y > 0 && map[x][y-1] == 0)
			client->Move(0, -1);
		break;
	case MV_DOWN: if ((y < WORLD_HEIGHT - 1) && (map[x][y + 1] == 0))
		client->Move(0, 1);
		break;
	case MV_LEFT: if (x > 0 && (map[x-1][y] == 0))
		client->Move(-1, 0);
		break;
	case MV_RIGHT: if ((x < WORLD_WIDTH - 1) && (map[x+1][y] == 0))
		client->Move(1, 0);
		break;
	default: std::cout << "Unknown Direction in CS_MOVE packet.\n";
		while (true);
	}
}

void CMap::RandomMove(CMonster* npc)
{
	int x = npc->GetInfo()->x;
	int y = npc->GetInfo()->y;
	switch (rand() % 4)
	{
	case 0: if (x > 0 && (map[x - 1][y] == 0))
		--x;
		break;
	case 1: if ((x < WORLD_WIDTH - 1) && (map[x + 1][y] == 0))
		++x;
		break;
	case 2: if ((y < WORLD_HEIGHT - 1) && (map[x][y + 1] == 0))
		--y;
		break;
	case 3:
		if (y > 0 && map[x][y - 1] == 0)
			++y;
		break;
	}
	npc->GetInfo()->x = x;
	npc->GetInfo()->y = y;
}
