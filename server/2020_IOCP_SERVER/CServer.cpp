#include "CServer.h"

CServer::CServer()
{
	dbConnector = new CDBConnector();
	map = new CMap("map.txt");
}

CServer::~CServer()
{
	delete dbConnector;
	delete map;
	for (auto i = characters.begin(); i != characters.end(); ++i)
		delete i->second;
	characters.clear();
	delete CTimer::GetInstance();
}

void CServer::run()
{
	std::wcout.imbue(std::locale("korean"));

	dbConnector->Init();

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	g_lSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_lSocket), h_iocp, KEY_SERVER, 0);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	::bind(g_lSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	::listen(g_lSocket, 5);

	SOCKET cSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<int>(cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));
	AcceptEx(g_lSocket, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);

	CTimer::GetInstance()->SetHandle(h_iocp);
	initialize_NPC();

	std::vector <std::thread> worker_threads;
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back([&]() {worker_thread(); });
	CTimer::GetInstance()->join();
	for (auto& th : worker_threads)
		th.join();

	dbConnector->Release();
	closesocket(g_lSocket);
	::WSACleanup();
}

void CServer::initialize_NPC()
{
	std::cout << "Initializing NPCs\n";
	int x, y;
	for (int i = MAX_USER; i < MAX_USER + NUM_NPC; ++i)
	{
		char npc_name[50];
		sprintf_s(npc_name, "N%d", i);

		int x, y;
		map->GetValidPosition(x, y);
		CInfo* info = new CInfo(i, npc_name, x, y);
		info->level = rand() % 10 + 1;
		info->atk = info->level;
		info->hp = info->level * 10;
		info->isUse = false;
		auto monster = new CMonster(info);
		characters[i] = monster;

		lua_register(monster->GetLua(), "API_SendEnterMessage", API_SendEnterMessage);
		lua_register(monster->GetLua(), "API_SendLeaveMessage", API_SendLeaveMessage);
		lua_register(monster->GetLua(), "API_get_x", API_get_x);
		lua_register(monster->GetLua(), "API_get_y", API_get_y);
	}
	std::cout << "NPC initialize finished.\n";
}

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s[%d]: ", msg, err_no);
	std::wcout << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
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
		int ret = 
		GetQueuedCompletionStatus(h_iocp, &io_size, &iocp_key, &lpover, INFINITE);
		key = static_cast<int>(iocp_key);
		// cout << "Completion Detected" << endl;
		if (FALSE == ret) {
			//error_display("GQCS Error", WSAGetLastError());
		}

		OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(lpover);
		switch (over_ex->op_mode) {
		case OP_MODE_ACCEPT:
			add_new_client(static_cast<SOCKET>(over_ex->wsa_buf.len));
			break;
		case OP_MODE_RECV:
			if (0 == io_size)
				disconnect_client(key);
			else {
				//std::cout << "Packet from Client [" << key << "] - ioSize: " << io_size << "\"\n";
				process_recv(key, io_size);
			}
			break;
		case OP_MODE_SEND:
			delete over_ex;
			break;
		case OP_RANDOM_MOVE:
			if (characters[key]->GetInfo()->hp > 0)
				random_move_npc(key);
			delete over_ex;
			break;
		case OP_PLAYER_MOVE_NOTIFY:
			reinterpret_cast<CMonster*>(characters[key])->MoveNotify(over_ex->object_id);
			delete over_ex;
			break;
		case OP_HEAL:
		{
			auto client = reinterpret_cast<CClient*>(characters[key]);
			client->AutoHeal();
			CTimer::GetInstance()->add_timer(key, OP_HEAL, std::chrono::system_clock::now() + std::chrono::seconds(5));
			delete over_ex;
		}
			break;
		}
	}
}

void CServer::random_move_npc(int id)
{
	auto npc = reinterpret_cast<CMonster*>(CServer::characters[id]);
	std::unordered_set <int> old_viewlist = characters[id]->GetViewlist();

	map->RandomMove(npc);

	std::unordered_set <int> new_viewlist;
	for (auto i = characters.cbegin(); i != characters.cend(); ++i)
	{
		if (i->first < MAX_USER)
			if (is_near(id, i->first))
				new_viewlist.insert(i->first);
	}

	for (auto pl : old_viewlist) {
		auto player = reinterpret_cast<CClient*>(CServer::characters[pl]);
		if (0 < new_viewlist.count(pl)) {
			if (0 < player->GetViewlist().count(id))
				player->send_move_packet(npc);
			else
				player->EnterPlayer(npc);
		}
		else
		{
			if (0 < player->GetViewlist().count(id))
				player->ErasePlayer(id);
		}
	}

	for (auto pl : new_viewlist) {
		auto player = reinterpret_cast<CClient*>(CServer::characters[pl]);
		if (0 == player->GetViewlist().count(pl))
		{
			if (0 == player->GetViewlist().count(id))
				player->EnterPlayer(npc);
			else
				player->send_move_packet(npc);
		}
	}

	if (new_viewlist.empty())
		npc->GetInfo()->isUse = false;
	else
		CTimer::GetInstance()->add_timer(id, OP_RANDOM_MOVE, std::chrono::system_clock::now() + std::chrono::seconds(1));

	for (auto pc : new_viewlist) {
		OVER_EX* over_ex = new OVER_EX;
		over_ex->object_id = pc;
		over_ex->op_mode = OP_PLAYER_MOVE_NOTIFY;
		PostQueuedCompletionStatus(h_iocp, 1, id, &over_ex->wsa_over);
	}
}

void CServer::add_new_client(SOCKET ns)
{
	int i;
	id_lock.lock();
	for (i = 0; i < MAX_USER; ++i)
	{
		if (0 == characters.count(i)) break;
		else
		{
			if (!characters[i]->GetInfo()->isUse)
			{
				delete characters[i];
				break;
			}
		}
	}
	id_lock.unlock();
	if (MAX_USER == i) {
		std::cout << "Max user limit exceeded.\n";
		closesocket(ns);
	}
	else {
		//std::cout << "New Client [" << i << "] Accepted" << std::endl;
		CClient* client = new CClient(i, "", 
			rand() % WORLD_WIDTH, 
			rand() % WORLD_HEIGHT, ns);
		client->GetInfo()->atk = client->GetInfo()->level * 10;
		characters[i] = client;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(ns), h_iocp, i, 0);
		client->StartRecv();
	}

	SOCKET cSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_accept_over.op_mode = OP_MODE_ACCEPT;
	g_accept_over.wsa_buf.len = static_cast<ULONG> (cSocket);
	ZeroMemory(&g_accept_over.wsa_over, sizeof(&g_accept_over.wsa_over));
	AcceptEx(g_lSocket, cSocket, g_accept_over.iocp_buf, 0, 32, 32, NULL, &g_accept_over.wsa_over);
}

void CServer::disconnect_client(int id)
{
	auto client = reinterpret_cast<CClient*>(characters[id]);
	for (auto i = characters.begin(); i != characters.end(); ++i)
	{
		if (i->first < MAX_USER)
		{
			if (i->first != id)
			{
				if (0 != i->second->GetViewlist().count(id))
				{
					i->second->GetViewlist().erase(id);
					reinterpret_cast<CClient*>(i->second)->ErasePlayer(id);
				}
			}
		}
	}

	client->GetInfo()->c_lock.lock();
	dbConnector->set_userdata(client, false);
	client->Release();
	client->GetInfo()->c_lock.unlock();
}

void CServer::wake_up_npc(int id)
{
	CTimer::GetInstance()->add_timer(id, OP_RANDOM_MOVE, std::chrono::system_clock::now() + std::chrono::seconds(1));
	//std::cout << "wake_up_npc called id - " << id << std::endl;
}

bool CServer::is_near(int p1, int p2)
{
	short x1 = characters[p1]->GetInfo()->x;
	short x2 = characters[p2]->GetInfo()->x;
	short y1 = characters[p1]->GetInfo()->y;
	short y2 = characters[p2]->GetInfo()->y;
	int dist = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);

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
	unsigned char p_size = client->getPacketStart()[0];
	unsigned char* next_recv_ptr = client->getRecvStart() + iosize;
	while (p_size <= next_recv_ptr - client->getPacketStart()) {
		process_packet(id);
		client->getPacketStart() += p_size;
		if (client->getPacketStart() < next_recv_ptr)
			p_size = client->getPacketStart()[0];
		else break;
	}

	long long left_data = next_recv_ptr - client->getPacketStart();

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
		if (client->GetInfo()->move_time < p->move_time) {
			client->GetInfo()->move_time = p->move_time;
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
	for (auto i = characters.begin(); i!=characters.end();++i)
	{
		if (id != i->first)
		{
			if (strcmp(i->second->GetInfo()->name.c_str(), client->GetInfo()->name.c_str()) != 0);
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
	for (auto i = characters.begin(); i != characters.end(); ++i)
	{
		if (is_near(id, i->first))
		{
			//npc case
			if (i->first > MAX_USER)
			{
				client->EnterPlayer(i->second);
				wake_up_npc(i->first);
			}
			//player case
			else if (id != i->first)
			{
				if (0 == i->second->GetViewlist().count(id))
					reinterpret_cast<CClient*>(i->second)->EnterPlayer(client);
				if (0 == client->GetViewlist().count(i->first))
					client->EnterPlayer(i->second);
			}
		}
	}
}

void CServer::process_move(int id, char dir)
{
	auto client = reinterpret_cast<CClient*>(CServer::characters[id]);
	std::unordered_set <int> old_viewlist = client->GetViewlist();

	map->ProcessMove(client, dir);
	client->send_move_packet(client);

	std::unordered_set <int> new_viewlist;
	for (auto i = characters.cbegin(); i != characters.cend(); ++i)
	{
		if (id == i->first)continue;
		if (is_near(id, i->first))
		{
			new_viewlist.insert(i->first);
			if (i->first > MAX_USER && !(old_viewlist.count(i->first)))
				wake_up_npc(i->first);
		}
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

	for (auto& npc : new_viewlist)
	{
		if (is_npc(npc))
		{
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
				auto npc = reinterpret_cast<CMonster*>(characters[i]);
				if (npc->GetDamage(client->GetInfo()->atk))
				{
					sprintf_s(mess, MAX_STR_LEN, "%s had %d damage. %d left",
						npc->GetInfo()->name.c_str(), client->GetInfo()->atk, npc->GetInfo()->hp);
				}
				client->send_chat_packet(i, mess);

				if (npc->GetInfo()->hp <= 0) {
					CTimer::GetInstance()->add_timer(i, OP_REVIVAL, std::chrono::system_clock::now() + std::chrono::seconds(30));
					sprintf_s(mess, MAX_STR_LEN, "%s has dead, %d exp gain",
						npc->GetInfo()->name.c_str(), npc->GetInfo()->level * 10);
					client->LevelUp(i, npc->GetInfo()->level * 10);
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

	if (monster->GetInfo()->isUse)
	{
		if (std::chrono::system_clock::now().time_since_epoch().count() > monster->GetInfo()->atk_time)
		{
			monster->GetInfo()->atk_time = std::chrono::system_clock::now().time_since_epoch().count();
			client->GetDamage(monster->GetInfo()->atk);
			CTimer::GetInstance()->add_timer(user_id, OP_HEAL, std::chrono::system_clock::now() + std::chrono::seconds(5));

			if (client->GetInfo()->hp <= 0)
			{
				client->Teleport(0, 0);
				client->GetInfo()->exp /= 2;
				client->GetInfo()->hp = client->GetInfo()->level * 70;
			}

			client->send_stat_change();
			char tmp[MAX_STR_LEN];
			sprintf_s(tmp, "You hit by id - %s", monster->GetInfo()->name.c_str());
			client->send_chat_packet(user_id, tmp);
		}

		client->send_chat_packet(my_id, mess);
		return 0;
	}

	return 0;
}

int CServer::API_SendLeaveMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	lua_pop(L, 3);

	CTimer::GetInstance()->add_timer(my_id, OP_RUNAWAY, std::chrono::system_clock::now() + std::chrono::seconds(3), user_id, mess);

	return 0;
}
