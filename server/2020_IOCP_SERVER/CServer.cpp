#include "CServer.h"

CServer::CServer()
{
	g_clients = new CClient[MAX_USER + NUM_NPC];
	timer = new CTimer();
	npcController = new CNPCController();
}

CServer::~CServer()
{
	delete timer;
	delete npcController;
	delete[] g_clients;
}

void CServer::run()
{
	std::wcout.imbue(std::locale("korean"));
	for (int i = 0; i < MAX_USER + NUM_NPC; ++i)
		g_clients[i].SetUse(false);

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
	timer = new CTimer;
	std::vector <std::thread> worker_threads;
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(&worker_thread);
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
		char npc_name[50];
		sprintf_s(npc_name, "N%d", i);

		g_clients->Init(rand() % WORLD_WIDTH,
						rand() % WORLD_HEIGHT,
						rand() % 10 + 1,
						npc_name, i);
	}
	std::cout << "NPC initialize finished.\n";
}

void CServer::worker_thread()
{
	// 반복
	//   - 이 쓰레드를 IOCP thread pool에 등록  => GQCS
	//   - iocp가 처리를 맞긴 I/O완료 데이터를 꺼내기 => GQCS
	//   - 꺼낸 I/O완료 데이터를 처리
	while (true) {
		DWORD io_size;
		int key;
		ULONG_PTR iocp_key;
		WSAOVERLAPPED* lpover;
		//int ret = 
		GetQueuedCompletionStatus(h_iocp, &io_size, &iocp_key, &lpover, INFINITE);
		key = static_cast<int>(iocp_key);
		// cout << "Completion Detected" << endl;
		//if (FALSE == ret) {
		//	error_display("GQCS Error : ", WSAGetLastError());
		//}

		OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(lpover);
		switch (over_ex->op_mode) {
		case OP_MODE_ACCEPT:
			add_new_client(static_cast<SOCKET>(over_ex->wsa_buf.len));
			break;
		case OP_MODE_RECV:
			if (0 == io_size)
				disconnect_client(key);
			else {
				// cout << "Packet from Client [" << key << "]" << endl;
				process_recv(key, io_size);
			}
			break;
		case OP_MODE_SEND:
			delete over_ex;
			break;
		case OP_RANDOM_MOVE:
			if (g_clients[key].getHP() > 0)
				npcController->random_move_npc(key);
			delete over_ex;
			break;
		case OP_PLAYER_MOVE_NOTIFY:
			g_clients[key].MoveNotify(over_ex->object_id);
			delete over_ex;
			break;
		case OP_HEAL:
		{
			timer->add_timer(key, OP_HEAL, std::chrono::system_clock::now() + std::chrono::seconds(5));
			delete over_ex;
		}
			break;
		}
	}
}

void CServer::add_new_client(SOCKET ns)
{
	int i;
	id_lock.lock();
	for (i = 0; i < MAX_USER; ++i)
		if (false == g_clients[i].getUse()) break;
	id_lock.unlock();
	if (MAX_USER == i) {
		std::cout << "Max user limit exceeded.\n";
		closesocket(ns);
	}
	else {
		// cout << "New Client [" << i << "] Accepted" << endl;
		g_clients[i].SetClient(i, ns);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(ns), h_iocp, i, 0);
		g_clients[i].StartRecv();
		timer->add_timer(i, OP_HEAL, std::chrono::system_clock::now() + std::chrono::seconds(5));
	}

	SOCKET cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<ULONG> (cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));
	AcceptEx(g_lSocket, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);
}

void CServer::disconnect_client(int id)
{
	for (auto& i : g_clients[id].getViewList())
		g_clients[i].ErasePlayer(id);
	g_clients[id].Release();
}

void CServer::wake_up_npc(int id)
{
	bool b = false;
	if (true == g_clients[id].CompareExchangeStrong(b))
	{
		timer->add_timer(id, OP_RANDOM_MOVE, std::chrono::system_clock::now() + std::chrono::seconds(1));
	}
}

bool CServer::is_near(int p1, int p2)
{
	int dist = (g_clients[p1].getX() - g_clients[p2].getX()) * (g_clients[p1].getX() - g_clients[p2].getX());
	dist += (g_clients[p1].getY() - g_clients[p2].getY()) * (g_clients[p1].getY() - g_clients[p2].getY());

	return dist <= VIEW_LIMIT * VIEW_LIMIT;
}

bool CServer::is_npc(int id)
{
	return id >= MAX_USER;
}

bool CServer::isIn_atkRange(int p1, int p2)
{
	int dist = (g_clients[p1].getX() - g_clients[p2].getX()) * (g_clients[p1].getX() - g_clients[p2].getX());
	dist += (g_clients[p1].getY() - g_clients[p2].getY()) * (g_clients[p1].getY() - g_clients[p2].getY());

	return dist <= 1;
}

void CServer::process_recv(int id, DWORD iosize)
{
	unsigned char* packet_start = g_clients[id].getPacketStart();
	unsigned char* next_recv_ptr = g_clients[id].getRecvStart() + iosize;
	unsigned char p_size = packet_start[0];
	while (p_size <= next_recv_ptr - packet_start) {
		process_packet(id);
		packet_start += p_size;
		if (packet_start < next_recv_ptr)
			p_size = packet_start[0];
		else break;
	}

	long long left_data = next_recv_ptr - packet_start;

	g_clients[id].IncreaseBuffer(iosize, left_data);
}

void CServer::process_packet(int id)
{
	char p_type = g_clients[id].getPacketType();
	switch (p_type) {
	case CS_LOGIN:
	{
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(g_clients[id].getPacketStart());

		strcpy_s(g_clients[id].getName(), MAX_ID_LEN, p->name);
		for (int i = 0; i < MAX_USER; ++i) {
			if (g_clients[i].getUse() && (i != id))
				if (strcmp(g_clients[i].getName(), g_clients[id].getName()) != 0);
				else {
					g_clients[id].send_login_fail();
					return;
				}
		}
		set_userdata(id, true);

		if (dbRetcode != SQL_SUCCESS && dbRetcode != SQL_SUCCESS_WITH_INFO)
			get_userdata(p, id);

		g_clients[id].send_login_ok();
		for (int i = 0; i < MAX_USER; ++i)
			if (true == g_clients[i].getUse())
				if (id != i) {
					if (false == is_near(i, id)) continue;
					if (0 == g_clients[i].getViewList().count(id))
						g_clients[i].EnterPlayer(g_clients[id]);
					if (0 == g_clients[id].getViewList().count(i))
						g_clients[id].EnterPlayer(g_clients[i]);
				}
		for (int i = MAX_USER; i < MAX_USER + NUM_NPC; ++i) {
			if (false == is_near(id, i)) continue;
			g_clients[id].EnterPlayer(g_clients[i]);
			wake_up_npc(i);
		}
		break;
	}
	case CS_MOVE: {
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(g_clients[id].getPacketStart());
		if (g_clients[id].getMoveTime() < p->move_time) {
			g_clients[id].getMoveTime() = p->move_time;
			process_move(id, p->direction);
		}
	}
				break;
	case CS_LOGOUT:
	{
		disconnect_client(id);
	}
	break;
	case CS_ATTACK:
	{
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(g_clients[id].getPacketStart());
		if (g_clients[id].getAtktime() < p->atk_time) {
			g_clients[id].getAtktime() = p->atk_time;
			process_attack(id);
		}
	}
	break;
	case CS_TELEPORT:
	{
		cs_packet_teleport* p = reinterpret_cast<cs_packet_teleport*>(g_clients[id].getPacketStart());
		g_clients[id].SetPosition(p->x, p->y);
	}
	break;
	default: std::cout << "Unknown Packet type [" << p_type << "] from Client [" << id << "]\n";
		while (true);
	}
}

void CServer::process_move(int id, char dir)
{
	short y = g_clients[id].getY();
	short x = g_clients[id].getX();
	switch (dir) {
	case MV_UP: if (y > 0) y--; break;
	case MV_DOWN: if (y < (WORLD_HEIGHT - 1)) y++; break;
	case MV_LEFT: if (x > 0) x--; break;
	case MV_RIGHT: if (x < (WORLD_WIDTH - 1)) x++; break;
	default: std::cout << "Unknown Direction in CS_MOVE packet.\n";
		while (true);
	}
	std::unordered_set <int> old_viewlist = g_clients[id].getViewList();

	g_clients[id].SetPosition(x, y);
	g_clients[id].send_move_packet(g_clients[id]);

	std::unordered_set <int> new_viewlist;
	for (int i = 0; i < MAX_USER; ++i) {
		if (id == i) continue;
		if (false == g_clients[i].getUse()) continue;
		if (true == is_near(id, i)) new_viewlist.insert(i);
	}

	for (int i = MAX_USER; i < MAX_USER + NUM_NPC; ++i) {
		if (true == is_near(id, i)) {
			new_viewlist.insert(i);
			wake_up_npc(i);
		}
	}

	// 시야에 들어온 객체 처리
	for (int ob : new_viewlist) {
		if (0 == old_viewlist.count(ob)) {
			g_clients[id].EnterPlayer(g_clients[ob]);

			if (false == is_npc(ob)) {
				if (0 == g_clients[ob].getViewList().count(id)) {
					g_clients[ob].EnterPlayer(g_clients[id]);
				}
				else {
					g_clients[ob].send_move_packet(g_clients[id]);
				}
			}
		}
		else {  // 이전에도 시야에 있었고, 이동후에도 시야에 있는 객체
			if (false == is_npc(ob)) {
				if (0 != g_clients[ob].getViewList().count(id)) {
					g_clients[ob].send_move_packet(g_clients[id]);
				}
				else
				{
					g_clients[ob].EnterPlayer(g_clients[id]);
				}
			}
		}
	}
	for (int ob : old_viewlist) {
		if (0 == new_viewlist.count(ob)) {
			g_clients[id].ErasePlayer(ob);
			if (false == is_npc(ob)) {
				if (0 != g_clients[ob].getViewList().count(id)) {
					g_clients[ob].ErasePlayer(id);
				}
			}
		}
	}

	if (false == is_npc(id)) {
		for (auto& npc : new_viewlist) {
			if (false == is_npc(npc)) continue;
			OVER_EX* ex_over = new OVER_EX;
			ex_over->object_id = id;
			ex_over->op_mode = OP_PLAYER_MOVE_NOTIFY;
			PostQueuedCompletionStatus(h_iocp, 1, npc, &ex_over->wsa_over);
		}
	}
}

void CServer::process_attack(int id)
{
	for (auto& i : g_clients[id].getViewList())
		if (isIn_atkRange(id, i)) {
			if (is_npc(i)) {
				char mess[MAX_STR_LEN];
				g_clients[i].HitByPlayer(mess);
				g_clients[id].send_chat_packet(id, mess);

				if (g_clients[i].getHP() <= 0) {
					timer->add_timer(i, OP_REVIVAL, std::chrono::system_clock::now() + std::chrono::seconds(30));
					sprintf_s(mess, "%s has dead, %d exp gain",
						g_clients[i].getName(), g_clients[i].getLevel() * 10);
				}
			}
		}
}
