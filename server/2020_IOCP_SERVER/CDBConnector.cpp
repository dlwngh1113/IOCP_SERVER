#include"CClient.h"
#include"CDBConnector.h"

CDBConnector::CDBConnector()
{
}

CDBConnector::~CDBConnector()
{
}

void CDBConnector::Init()
{
	dbRetcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	dbRetcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
	dbRetcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
	dbRetcode = SQLConnect(hdbc, (SQLWCHAR*)L"g_server_1", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
}


void CDBConnector::Release()
{
	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

SQLRETURN CDBConnector::GetReturnCode() const
{
	return dbRetcode;
}

void CDBConnector::set_userdata(CClient* client, bool isInit)
{
	dbRetcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	wchar_t tmp[MAX_STR_LEN] = { NULL };

	int nChars = MultiByteToWideChar(CP_ACP, 0, client->GetInfo()->name.c_str(), -1, NULL, 0);
	wchar_t* nameWchar = new wchar_t[nChars];
	MultiByteToWideChar(CP_ACP, 0, client->GetInfo()->name.c_str(), -1, (LPWSTR)nameWchar, nChars);

	if (isInit) {
		client->SetInfo(client->GetInfo()->name.c_str(), 1, 0, 0, 0, 100);
		wsprintf(tmp, L"EXEC insert_player %s", nameWchar);

		dbRetcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp, SQL_NTS);

		return;
	}

	//이름, 레벨, x, y, exp
	wsprintf(tmp, L"EXEC set_userdata %s, %d, %hd, %hd, %d, %hd",
		nameWchar,
		client->GetInfo()->name.c_str(),
		client->GetInfo()->level,
		client->GetInfo()->x,
		client->GetInfo()->y,
		client->GetInfo()->exp);

	dbRetcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp, SQL_NTS);

	delete[] nameWchar;
}

void CDBConnector::get_userdata(CClient* client, cs_packet_login* p)
{
	dbRetcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	char getQuery[MAX_STR_LEN] = { NULL };
	sprintf_s(getQuery, "EXEC get_userdata %s", p->name);

	int nChars = MultiByteToWideChar(CP_ACP, 0, getQuery, -1, NULL, 0);
	wchar_t* pwcsName = new wchar_t[nChars];
	MultiByteToWideChar(CP_ACP, 0, getQuery, -1, (LPWSTR)pwcsName, nChars);

	dbRetcode = SQLExecDirect(hstmt, (SQLWCHAR*)pwcsName, SQL_NTS);

	if (dbRetcode == SQL_SUCCESS || dbRetcode == SQL_SUCCESS_WITH_INFO) {
		SQLINTEGER LEVEL, EXP;
		SQLSMALLINT POSX, POSY, HP;
		SQLLEN cbLevel, cbExp, cbPosX, cbPosY, cbHp;
		// Bind columns 1, 2, and 3  
		dbRetcode = SQLBindCol(hstmt, 1, SQL_C_LONG, &LEVEL, 100, &cbLevel);
		dbRetcode = SQLBindCol(hstmt, 2, SQL_C_SHORT, &POSX, 100, &cbPosX);
		dbRetcode = SQLBindCol(hstmt, 3, SQL_C_SHORT, &POSY, 100, &cbPosY);
		dbRetcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &EXP, 100, &cbExp);
		dbRetcode = SQLBindCol(hstmt, 5, SQL_C_SHORT, &HP, 100, &cbHp);

		dbRetcode = SQLFetch(hstmt);
		if (dbRetcode == SQL_ERROR || dbRetcode == SQL_SUCCESS_WITH_INFO)
			std::cout << "code line 388 error\n";
		if (dbRetcode == SQL_SUCCESS || dbRetcode == SQL_SUCCESS_WITH_INFO)
		{
			client->SetInfo(p->name, LEVEL, POSX, POSY, EXP, HP);
		}
	}

	delete[] pwcsName;
}
