#pragma once
#include"framework.h"

class CDBManager
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
	SQLRETURN dbRetcode;

	static CDBManager* instance;
public:
	CDBManager();
	virtual ~CDBManager();

#pragma region getter
	CDBManager* GetInstance();
#pragma endregion

};

