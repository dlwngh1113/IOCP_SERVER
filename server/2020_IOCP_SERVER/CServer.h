#pragma once
#include"framework.h"
#include"CClient.h"
#include"CTimer.h"
class CServer
{
	CClient g_clients[MAX_USER + NUM_NPC];
	HANDLE		h_iocp;

	SOCKET g_lSocket;
	OVER_EX g_accept_over;

	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN dbRetcode;

	CTimer* timer;
public:
	CServer();
	virtual ~CServer();
	void run();
	void initialize_NPC();
	void worker_thread();
	void add_new_client(SOCKET ns);
	void disconnect_client(int id);
};

