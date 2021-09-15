#pragma once
#include"framework.h"
#include"CClient.h"

class CDBConnector
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN dbRetcode;
public:
	CDBConnector();
	virtual ~CDBConnector();

	void Init();
	void Release();

	SQLRETURN GetReturnCode() const;

	void set_userdata(CClient* client, bool isInit);
	void get_userdata(CClient* client, cs_packet_login* p);
};

