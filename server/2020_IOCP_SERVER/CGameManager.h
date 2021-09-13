#pragma once
class CGameManager
{
	static CGameManager* instance;
public:
	CGameManager();
	virtual ~CGameManager();
};