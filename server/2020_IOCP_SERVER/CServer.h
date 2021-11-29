#pragma once
#include"CClient.h"
#include"CMonster.h"
#include"CTimer.h"
#include"CDBConnector.h"

class CServer
{
	HANDLE		h_iocp;

	SOCKET g_lSocket;
	OVER_EX g_accept_over;

	std::mutex id_lock;

	CDBConnector* dbConnector;
public:
	static concurrency::concurrent_unordered_map<int, CCharacter*> characters;
	CServer();
	virtual ~CServer();

	void run();
	void initialize_NPC();
	void worker_thread();
	void random_move_npc(int id);
	void add_new_client(SOCKET ns);
	void disconnect_client(int id);

	void wake_up_npc(int id);
	
	bool is_near(int p1, int p2);
	bool is_npc(int id);
	bool isIn_atkRange(int p1, int p2);

	void process_recv(int id, DWORD io_size);
	void process_packet(int id);
	void process_login(cs_packet_login* p, int id);
	void process_move(int id, char dir);
	void process_attack(int id);

	static int API_get_x(lua_State* L);
	static int API_get_y(lua_State* L);
	static int API_SendEnterMessage(lua_State* L);
	static int API_SendLeaveMessage(lua_State* L);
};