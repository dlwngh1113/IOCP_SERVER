#include "CClient.h"

CClient::CClient()
{
	name = new char[MAX_ID_LEN];
}

CClient::~CClient()
{
	delete[] name;
}

void CClient::SetUse(bool b)
{
	in_use = b;
}

void CClient::SetClient(int id, SOCKET ns)
{
	this->c_lock.lock();
	this->in_use = true;
	this->m_sock = ns;
	this->name[0] = 0;
	this->c_lock.unlock();

	this->m_packet_start = this->m_recv_over.iocp_buf;
	this->m_recv_over.op_mode = OP_MODE_RECV;
	this->m_recv_over.wsa_buf.buf
		= reinterpret_cast<CHAR*>(this->m_recv_over.iocp_buf);
	this->m_recv_over.wsa_buf.len = sizeof(this->m_recv_over.iocp_buf);
	ZeroMemory(&this->m_recv_over.wsa_over, sizeof(this->m_recv_over.wsa_over));
	this->m_recv_start = this->m_recv_over.iocp_buf;

	this->x = rand() % WORLD_WIDTH;
	this->y = rand() % WORLD_HEIGHT;
}

void CClient::SetInfo(char* name, short level, short x, short y, int exp, short hp)
{
	this->c_lock.lock();
	strcpy_s(this->name, MAX_ID_LEN, name);

	this->level = level;
	this->x = x;
	this->y = y;
	this->exp = exp;
	this->hp = hp;
	this->c_lock.unlock();
}

void CClient::SetPosition(short x, short y)
{
	this->x = x;
	this->y = y;
}

void CClient::MoveNotify(int objID)
{
	this->lua_l.lock();
	lua_getglobal(this->L, "event_player_move");
	lua_pushnumber(this->L, objID);
	lua_pcall(this->L, 1, 1, 0);
	this->lua_l.unlock();
}

void CClient::Init(short x, short y, short level, char* name, int i)
{
	this->x = x;
	this->y = y;
	this->level = level;
	this->hp = level * 100;
	strcpy_s(this->name, MAX_ID_LEN, name);

	this->is_active = false;
	this->L = luaL_newstate();

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

void CClient::Release()
{
	this->c_lock.lock();
	set_userdata(id, false);
	this->in_use = false;
	this->view_list.clear();
	closesocket(this->m_sock);
	this->m_sock = 0;
	this->c_lock.unlock();
}

void CClient::AutoHeal()
{
	this->c_lock.lock();
	short maxHp = this->level * 70;
	if (this->hp + maxHp * 0.1 >= maxHp)
		this->hp = maxHp;
	else if (this->hp + maxHp * 0.1 < maxHp)
		this->hp += maxHp * 0.1;
	char mess[MAX_STR_LEN];
	sprintf_s(mess, "auto healing...%s", this->name);
	send_heal_packet(mess);
	this->c_lock.unlock();
}

void CClient::LevelUp(int targetID, int exp)
{
	c_lock.lock();
	send_leave_packet(targetID);
	this->exp += exp;
	if (this->exp > level * 100) {
		++level;
		this->exp -= this->level * 100;
		this->hp = this->level * 70;
	}
	send_stat_change();
	c_lock.unlock();
}

void CClient::HitByPlayer(char* mess)
{
	c_lock.lock();
	hp -= 100;
	sprintf_s(mess, MAX_STR_LEN, "%s had %d damage. %d left",
		name, 100, hp);
	c_lock.unlock();
}

void CClient::send_login_fail()
{
	sc_packet_login_fail p;
	p.id = id;
	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	strcpy_s(p.message, "another client is using this name");
	send_packet(&p);
}

void CClient::send_login_ok()
{
	sc_packet_login_ok p;
	p.exp = exp;
	p.hp = hp;
	p.id = id;
	p.level = level;
	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	p.x = x;
	p.y = y;
	send_packet(&p);
}

void CClient::send_heal_packet(char* mess)
{
	sc_packet_stat_change p;
	p.level = this->level;
	p.exp = this->exp;
	p.hp = this->hp;
	p.type = SC_PACKET_STAT_CHANGE;
	strcpy_s(p.message, mess);
	p.size = sizeof(p);
	send_packet(&p);
}

void CClient::send_leave_packet(int targetID)
{
	sc_packet_leave p;
	p.id = targetID;
	p.size = sizeof(p);
	p.type = SC_PACKET_LEAVE;
	send_packet(&p);
}

void CClient::send_enter_packet(CClient& other)
{
	sc_packet_enter p;
	p.id = other.id;
	p.size = sizeof(p);
	p.type = SC_PACKET_ENTER;
	p.x = other.x;
	p.y = other.y;
	other.c_lock.lock();
	strcpy_s(p.name, other.name);
	other.c_lock.unlock();
	p.o_type = 0;
	send_packet(&p);
}

void CClient::send_move_packet(CClient& other)
{
	sc_packet_move p;
	p.id = id;
	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE;
	p.x = other.x;
	p.y = other.y;
	p.move_time = other.move_time;
	send_packet(&p);
}

void CClient::send_stat_change()
{
	sc_packet_stat_change p;
	p.level = level;
	p.exp = exp;
	p.hp = hp;
	p.type = SC_PACKET_STAT_CHANGE;
	p.size = sizeof(p);
	send_packet(&p);
}

void CClient::send_chat_packet(int targetID, char* mess)
{
		sc_packet_chat p;
		p.id = targetID;
		p.size = sizeof(p);
		p.type = SC_PACKET_CHAT;
		strcpy_s(p.message, mess);
		send_packet(&p);
}

bool CClient::CompareExchangeStrong(bool b)
{
	return is_active.compare_exchange_strong(b, true);
}

void CClient::StartRecv()
{
	DWORD flags = 0;
	int ret;
	this->c_lock.lock();
	if (true == this->in_use) {
		ret = WSARecv(this->m_sock, &this->m_recv_over.wsa_buf, 1, NULL,
			&flags, &this->m_recv_over.wsa_over, NULL);
	}
	this->c_lock.unlock();
	if (SOCKET_ERROR == ret) {
		int err_no = WSAGetLastError();
		if (ERROR_IO_PENDING != err_no)
			std::cout << "WSARecv: " << err_no << std::endl;
			//error_display("WSARecv : ", err_no);
	}
}

void CClient::ErasePlayer(int id)
{
	this->vl.lock();
	this->view_list.erase(id);
	this->vl.unlock();
	send_leave_packet(id);
}

void CClient::EnterPlayer(CClient& other)
{
	vl.lock();
	view_list.insert(other.id);
	send_enter_packet(other);
	vl.unlock();
}

void CClient::IncreaseBuffer(DWORD iosize, long long left_data)
{
	unsigned char* next_recv_ptr = m_recv_start + iosize;
	if ((MAX_BUFFER - (next_recv_ptr - m_recv_over.iocp_buf))
		< MIN_BUFF_SIZE) {
		memcpy(m_recv_over.iocp_buf,
			m_packet_start, left_data);
		m_packet_start = m_recv_over.iocp_buf;
		next_recv_ptr = m_packet_start + left_data;
	}
	DWORD recv_flag = 0;
	m_recv_start = next_recv_ptr;
	m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(next_recv_ptr);
	m_recv_over.wsa_buf.len = MAX_BUFFER -
		static_cast<int>(next_recv_ptr - m_recv_over.iocp_buf);

	c_lock.lock();
	if (true == in_use) {
		WSARecv(m_sock, &m_recv_over.wsa_buf,
			1, NULL, &recv_flag, &m_recv_over.wsa_over, NULL);
	}
	c_lock.unlock();
}

void CClient::send_packet(void* p)
{
	unsigned char* packet = reinterpret_cast<unsigned char*>(p);
	OVER_EX* send_over = new OVER_EX;
	memcpy(send_over->iocp_buf, packet, packet[0]);
	send_over->op_mode = OP_MODE_SEND;
	send_over->wsa_buf.buf = reinterpret_cast<CHAR*>(send_over->iocp_buf);
	send_over->wsa_buf.len = packet[0];
	ZeroMemory(&send_over->wsa_over, sizeof(send_over->wsa_over));
	this->c_lock.lock();
	if (true == this->in_use)
		WSASend(this->m_sock, &send_over->wsa_buf, 1,
			NULL, 0, &send_over->wsa_over, NULL);
	this->c_lock.unlock();
}

short CClient::getHP() const
{
	return this->hp;
}

bool CClient::getUse() const
{
	return this->in_use;
}

char* CClient::getName()
{
	return name;
}

short CClient::getLevel() const
{
	return level;
}

int CClient::getExp() const
{
	return exp;
}

std::unordered_set<int>& CClient::getViewList()
{
	return this->view_list;
}

int& CClient::getAtktime()
{
	return atk_time;
}

int& CClient::getMoveTime()
{
	return move_time;
}

unsigned char* CClient::getPacketStart()
{
	return m_packet_start;
}

unsigned char* CClient::getRecvStart()
{
	return m_recv_start;
}

char CClient::getPacketType() const
{
	return m_packet_start[1];
}

short CClient::getX() const
{
	return x;
}

short CClient::getY() const
{
	return y;
}
