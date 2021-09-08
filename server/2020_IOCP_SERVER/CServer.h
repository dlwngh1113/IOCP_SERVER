#pragma once
#include"framework.h"
#include"CClient.h"
#include"CTimer.h"
#include"CNPCController.h"
class CServer
{
	CClient* g_clients;
	HANDLE		h_iocp;

	SOCKET g_lSocket;
	OVER_EX g_accept_over;

	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN dbRetcode;

	std::mutex id_lock;

	CTimer* timer;
	CNPCController* npcController;
public:
	CServer();
	virtual ~CServer();
	void run();
	void initialize_NPC();
	void worker_thread();
	void add_new_client(SOCKET ns);
	void disconnect_client(int id);

	void wake_up_npc(int id);
	
	bool is_near(int p1, int p2);
	bool is_npc(int id);

	void process_recv(int id, DWORD io_size);
	void process_packet(int id);
	void process_move(int id, char dir);
};

