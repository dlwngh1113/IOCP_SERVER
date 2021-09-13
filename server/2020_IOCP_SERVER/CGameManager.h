#pragma once
class CGameManager
{
	static CGameManager* Instance;
public:
	CGameManager();
	virtual ~CGameManager();

#pragma region getter
	CGameManager* GetInstance() const;
#pragma endregion

#pragma region setter
	void SetInstance();
#pragma endregion

};