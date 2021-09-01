#include "CServer.h"

CServer::CServer()
{
	timer = new CTimer();
}

CServer::~CServer()
{
	delete timer;
}

void CServer::run()
{
	std::wcout.imbue(std::locale("korean"));
	for (auto& cl : g_clients)
		cl.SetUse(false);

	dbRetcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	dbRetcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
	dbRetcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
	dbRetcode = SQLConnect(hdbc, (SQLWCHAR*)L"g_server_1", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	g_lSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_lSocket), h_iocp, KEY_SERVER, 0);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	::bind(g_lSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(g_lSocket, 5);

	SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<int>(cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));
	AcceptEx(g_lSocket, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);

	initialize_NPC();

	//thread ai_thread{ npc_ai_thread };
	std::thread timer_thread{ time_worker };
	std::vector <std::thread> worker_threads;
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(worker_thread);
	for (auto& th : worker_threads)
		th.join();
	//ai_thread.join();
	timer->join();

	SQLCancel(hstmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	closesocket(g_lSocket);
	WSACleanup();
}

void CServer::initialize_NPC()
{
	std::cout << "Initializing NPCs\n";
	for (int i = MAX_USER; i < MAX_USER + NUM_NPC; ++i)
	{
		g_clients[i].x = rand() % WORLD_WIDTH;
		g_clients[i].y = rand() % WORLD_HEIGHT;
		g_clients[i].level = rand() % 10 + 1;
		g_clients[i].hp = g_clients[i].level * 100;
		char npc_name[50];
		sprintf_s(npc_name, "N%d", i);
		strcpy_s(g_clients[i].name, npc_name);
		g_clients[i].is_active = false;

		lua_State* L = g_clients[i].L = luaL_newstate();
		luaL_openlibs(L);

		int error = luaL_loadfile(L, "monster.lua");
		error = lua_pcall(L, 0, 0, 0);

		lua_getglobal(L, "set_uid");
		lua_pushnumber(L, i);
		lua_pcall(L, 1, 1, 0);
		// lua_pop(L, 1);// eliminate set_uid from stack after call

		lua_register(L, "API_SendEnterMessage", API_SendEnterMessage);
		lua_register(L, "API_SendLeaveMessage", API_SendLeaveMessage);
		lua_register(L, "API_get_x", API_get_x);
		lua_register(L, "API_get_y", API_get_y);
	}
	std::cout << "NPC initialize finished.\n";
}
