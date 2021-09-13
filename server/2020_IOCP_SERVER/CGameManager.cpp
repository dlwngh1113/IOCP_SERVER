#include "CGameManager.h"

CGameManager::CGameManager()
{
}

CGameManager::~CGameManager()
{
}

CGameManager* CGameManager::GetInstance() const
{
	if (Instance != nullptr)
	{
		return Instance;
	}
	else
	{
		return nullptr;
	}
}

void CGameManager::SetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new CGameManager();
	}
	else
	{
		delete this;
	}
}
