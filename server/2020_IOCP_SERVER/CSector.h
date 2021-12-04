#pragma once
#include"framework.h"
class CSector
{
	std::unordered_set<int> objects;
	std::mutex sectorLock;
public:
	CSector() = default;
	virtual ~CSector();

	void AddInSector(int id);
	void RemoveFromSector(int id);
	void IsInSector(int id);
};