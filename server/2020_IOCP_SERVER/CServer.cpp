#include "CServer.h"

CServer::CServer()
{
	timer = new CTimer(h_iocp);
	npcController = new CNPCController();
	dbConnector = new CDBConnector();
}

CServer::~CServer()
{
	delete timer;
	delete npcController;
	delete dbConnector;
}

void CServer::run()
{
	std::wcout.imbue(std::locale("korean"));

	dbConnector->Init();

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

	dbConnector->Release();
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

		auto monster = new CMonster(i, npc_name, rand() % WORLD_WIDTH, rand() % WORLD_HEIGHT, rand() % 10 + 1);
		characters[i] = monster;

		lua_register(monster->GetLua(), "API_SendEnterMessage", API_SendEnterMessage);
		lua_register(monster->GetLua(), "API_SendLeaveMessage", API_SendLeaveMessage);
		lua_register(monster->GetLua(), "API_get_x", API_get_x);
		lua_register(monster->GetLua(), "API_get_y", API_get_y);
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
			if (reinterpret_cast<CClient*>(characters[key])->GetInfo()->hp > 0)
				npcController->random_move_npc(key);
			delete over_ex;
			break;
		case OP_PLAYER_MOVE_NOTIFY:
			reinterpret_cast<CMonster*>(characters[key])->MoveNotify(over_ex->object_id);
			delete over_ex;
			break;
		case OP_HEAL:
		{
			reinterpret_cast<CClient*>(characters[key])->AutoHeal();
			timer->add_timer(key, OP_HEAL, std::chrono::system_clock::now() + std::chrono::seconds(5));
			delete over_ex;
		}
			break;
		case OP_REVIVAL:
		{
			auto player = reinterpret_cast<CClient*>(characters[key]);
			player->GetInfo()->level = rand() % 10 + 1;
			player->GetInfo()->hp = player->GetInfo()->level * 100;
			for (int i = 0; i < MAX_USER; ++i)
				if (is_near(key, i))
					player->EnterPlayer(characters[i]);
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
		if (0 == characters.count(i)) break;
	id_lock.unlock();
	if (MAX_USER == i) {
		std::cout << "Max user limit exceeded.\n";
		closesocket(ns);
	}
	else {
		// cout << "New Client [" << i << "] Accepted" << endl;
		characters[i] = new CClient(i, "", 
			rand() % WORLD_WIDTH, 
			rand() % WORLD_HEIGHT, ns);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(ns), h_iocp, i, 0);
		reinterpret_cast<CClient*>(characters[i])->StartRecv();
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
	auto client = reinterpret_cast<CClient*>(characters[id]);
	for (const auto& i : client->GetViewlist())
		client->ErasePlayer(id);
	dbConnector->set_userdata(client, false);
	client->Release();
}

void CServer::wake_up_npc(int id)
{
	timer->add_timer(id, OP_RANDOM_MOVE, std::chrono::system_clock::now() + std::chrono::seconds(1));
}

bool CServer::is_near(int p1, int p2)
{
	int dist = (characters[p1]->GetInfo()->x - characters[p2]->GetInfo()->x) * (characters[p1]->GetInfo()->x - characters[p2]->GetInfo()->x);
	dist += (characters[p1]->GetInfo()->y - characters[p2]->GetInfo()->y) * (characters[p1]->GetInfo()->y - characters[p2]->GetInfo()->y);

	return dist <= VIEW_LIMIT * VIEW_LIMIT;
}

bool CServer::is_npc(int id)
{
	return id >= MAX_USER;
}

bool CServer::isIn_atkRange(int p1, int p2)
{
	int dist = (characters[p1]->GetInfo()->x - characters[p2]->GetInfo()->x) * (characters[p1]->GetInfo()->x - characters[p2]->GetInfo()->x);
	dist += (characters[p1]->GetInfo()->y - characters[p2]->GetInfo()->y) * (characters[p1]->GetInfo()->y - characters[p2]->GetInfo()->y);

	return dist <= 1;
}

void CServer::process_recv(int id, DWORD iosize)
{
	auto client = reinterpret_cast<CClient*>(characters[id]);
	unsigned char* packet_start = client->getPacketStart();
	unsigned char* next_recv_ptr = client->getRecvStart() + iosize;
	unsigned char p_size = packet_start[0];
	while (p_size <= next_recv_ptr - packet_start) {
		process_packet(id);
		packet_start += p_size;
		if (packet_start < next_recv_ptr)
			p_size = packet_start[0];
		else break;
	}

	long long left_data = next_recv_ptr - packet_start;

	client->IncreaseBuffer(iosize, left_data);
}

void CServer::process_packet(int id)
{
	auto client = reinterpret_cast<CClient*>(characters[id]);
	char p_type = client->getPacketType();
	switch (p_type) {
	case CS_LOGIN:
	{
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(client->getPacketStart());
		process_login(p, id);
	}
	break;
	case CS_MOVE: 
	{
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(client->getPacketStart());
		if (client->getMoveTime() < p->move_time) {
			client->getMoveTime() = p->move_time;
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
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(client->getPacketStart());
		if (client->GetInfo()->atk_time < p->atk_time) {
			client->GetInfo()->atk_time = p->atk_time;
			process_attack(id);
		}
	}
	break;
	case CS_TELEPORT:
	{
		cs_packet_teleport* p = reinterpret_cast<cs_packet_teleport*>(client->getPacketStart());
		client->Teleport(p->x, p->y);
	}
	break;
	default: std::cout << "Unknown Packet type [" << p_type << "] from Client [" << id << "]\n";
		while (true);
	}
}

void CServer::process_login(cs_packet_login* p, int id)
{
	auto client = reinterpret_cast<CClient*>(characters[id]);
	client->GetInfo()->name = p->name;
	for (const auto& c : characters)
	{
		if (id != c.first)
		{
			if (strcmp(c.second->GetInfo()->name.c_str(), client->GetInfo()->name.c_str()) != 0);
			else
			{
				client->send_login_fail();
				return;
			}
		}
	}
	dbConnector->set_userdata(client, true);

	if (dbConnector->GetReturnCode() != SQL_SUCCESS && dbConnector->GetReturnCode() != SQL_SUCCESS_WITH_INFO)
		dbConnector->get_userdata(client, p);

	client->send_login_ok();
	for (const auto& ch : characters)
	{
		//npc case
		if (ch.first > MAX_USER)
		{
			if (false == is_near(id, ch.first))continue;
			client->EnterPlayer(reinterpret_cast<CClient*>(ch.second));
			wake_up_npc(ch.first);
		}
		//player case
		else if (id != ch.first)
		{
			if (false == is_near(ch.first, id))continue;
			if (0 == ch.second->GetViewlist().count(id))
				reinterpret_cast<CClient*>(ch.second)->EnterPlayer(client);
			if (0 == client->GetViewlist().count(ch.first))
				client->EnterPlayer(reinterpret_cast<CClient*>(ch.second));
		}
	}
}

void CServer::process_move(int id, char dir)
{
	auto client = reinterpret_cast<CClient*>(characters[id]);
	short y = client->GetInfo()->x;
	short x = client->GetInfo()->y;
	switch (dir) {
	case MV_UP: if (y > 0)
		client->Move(0, -1);
		break;
	case MV_DOWN: if (y < (WORLD_HEIGHT - 1))
		client->Move(0, 1);
		break;
	case MV_LEFT: if (x > 0)
		client->Move(-1, 0);
		break;
	case MV_RIGHT: if (x < (WORLD_WIDTH - 1))
		client->Move(1, 0);
		break;
	default: std::cout << "Unknown Direction in CS_MOVE packet.\n";
		while (true);
	}
	std::unordered_set <int> old_viewlist = client->GetViewlist();

	client->send_move_packet(client);

	std::unordered_set <int> new_viewlist;
	for (const auto& ch : characters)
	{
		if (id == ch.first)continue;
		if (ch.first > MAX_USER)
		{
			if (true == is_near(id, ch.first))
			{
				new_viewlist.insert(ch.first);
				wake_up_npc(ch.first);
			}
		}
		else if (true == is_near(id, ch.first))
			new_viewlist.insert(ch.first);
	}

	// 시야에 들어온 객체 처리
	for (int ob : new_viewlist) {
		if (0 == old_viewlist.count(ob)) {
			client->EnterPlayer(characters[ob]);

			if (false == is_npc(ob)) {
				if (0 == characters[ob]->GetViewlist().count(id)) {
					reinterpret_cast<CClient*>(characters[ob])->EnterPlayer(characters[id]);
				}
				else 
				{
					reinterpret_cast<CClient*>(characters[ob])->send_move_packet(client);
				}
			}
		}
		else {  // 이전에도 시야에 있었고, 이동후에도 시야에 있는 객체
			if (false == is_npc(ob)) {
				if (0 != characters[ob]->GetViewlist().count(id)) {
					reinterpret_cast<CClient*>(characters[ob])->send_move_packet(client);
				}
				else
				{
					reinterpret_cast<CClient*>(characters[ob])->EnterPlayer(client);
				}
			}
		}
	}
	for (int ob : old_viewlist) {
		if (0 == new_viewlist.count(ob)) {
			client->ErasePlayer(ob);
			if (false == is_npc(ob)) {
				if (0 != characters[ob]->GetViewlist().count(id)) {
					reinterpret_cast<CClient*>(characters[ob])->ErasePlayer(id);
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
	auto client = reinterpret_cast<CClient*>(characters[id]);
	for (auto& i : client->GetViewlist())
		if (isIn_atkRange(id, i)) {
			if (is_npc(i)) {
				char mess[MAX_STR_LEN];
				if (characters[i]->GetDamage(client->GetInfo()->atk))
				{
					sprintf_s(mess, MAX_STR_LEN, "%s had %d damage. %d left",
						characters[i]->GetInfo()->name.c_str(), client->GetInfo()->atk, characters[i]->GetInfo()->hp);
				}
				client->send_chat_packet(i, mess);

				if (characters[i]->GetInfo()->hp <= 0) {
					timer->add_timer(i, OP_REVIVAL, std::chrono::system_clock::now() + std::chrono::seconds(30));
					sprintf_s(mess, "%s has dead, %d exp gain",
						characters[i]->GetInfo()->name.c_str(), characters[i]->GetInfo()->level * 10);
				}
			}
		}
}

int CServer::API_get_x(lua_State* L)
{
	int user_id = lua_tointeger(L, -1);
	lua_pop(L, 2);
	int x = characters[user_id]->GetInfo()->x;
	lua_pushnumber(L, x);
	return 1;
}

int CServer::API_get_y(lua_State* L)
{
	int user_id = lua_tointeger(L, -1);
	lua_pop(L, 2);
	int y = characters[user_id]->GetInfo()->y;
	lua_pushnumber(L, y);
	return 1;
}

int CServer::API_SendEnterMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	lua_pop(L, 3);

	auto monster = reinterpret_cast<CMonster*>(characters[my_id]);
	auto client = reinterpret_cast<CClient*>(characters[user_id]);

	if (std::chrono::system_clock::now().time_since_epoch().count() > monster->GetInfo()->atk_time)
	{
		monster->GetInfo()->atk_time = std::chrono::system_clock::now().time_since_epoch().count();
		client->GetDamage(monster->GetInfo()->atk);

		client->send_stat_change();
		char tmp[MAX_STR_LEN];
		sprintf_s(tmp, "You hit by id - %s", monster->GetInfo()->name.c_str());
		client->send_chat_packet(user_id, tmp);
	}

	client->send_chat_packet(my_id, mess);
	return 0;
}

int CServer::API_SendLeaveMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	lua_pop(L, 3);

	timer->add_timer(my_id, OP_RUNAWAY, std::chrono::system_clock::now() + std::chrono::seconds(3), user_id, mess);

	return 0;
}
