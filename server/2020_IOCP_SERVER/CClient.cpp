#include "CClient.h"

CClient::CClient(): CCharacter()
{
}

CClient::CClient(int id, std::string name, short x, short y, SOCKET s):CCharacter(id, name, x, y)
{
	c_lock.lock();
	m_sock = s;
	c_lock.unlock();

	this->m_packet_start = this->m_recv_over.iocp_buf;
	this->m_recv_over.op_mode = OP_MODE_RECV;
	this->m_recv_over.wsa_buf.buf
		= reinterpret_cast<CHAR*>(this->m_recv_over.iocp_buf);
	this->m_recv_over.wsa_buf.len = sizeof(this->m_recv_over.iocp_buf);
	ZeroMemory(&this->m_recv_over.wsa_over, sizeof(this->m_recv_over.wsa_over));
	this->m_recv_start = this->m_recv_over.iocp_buf;
}

CClient::CClient(CInfo* info) : CCharacter{ info }
{

}

CClient::~CClient()
{
}

void CClient::SetInfo(const char* name, short level, short x, short y, int exp, short hp)
{
	this->c_lock.lock();
	GetInfo()->name = name;

	GetInfo()->level = level;
	GetInfo()->x = x;
	GetInfo()->y = y;
	GetInfo()->exp = exp;
	GetInfo()->hp = hp;
	this->c_lock.unlock();
}

void CClient::Teleport(short x, short y)
{
	CCharacter::Teleport(x, y);
}

void CClient::Release()
{
	this->c_lock.lock();
	viewList.clear();
	closesocket(this->m_sock);
	this->m_sock = 0;
	this->c_lock.unlock();
}

void CClient::AutoHeal()
{
	this->c_lock.lock();
	short maxHp = GetInfo()->level * 70;
	if (GetInfo()->hp + maxHp * 0.1 >= maxHp)
		GetInfo()->hp = maxHp;
	else if (GetInfo()->hp + maxHp * 0.1 < maxHp)
		GetInfo()->hp += maxHp * 0.1;
	char mess[MAX_STR_LEN];
	sprintf_s(mess, "auto healing...%s", GetInfo()->name.c_str());
	send_heal_packet(mess);
	this->c_lock.unlock();
}

void CClient::LevelUp(int targetID, int exp)
{
	c_lock.lock();
	send_leave_packet(targetID);
	GetInfo()->exp += exp;
	if (GetInfo()->exp > GetInfo()->level * 100) {
		GetInfo()->level += 1;
		GetInfo()->exp -= GetInfo()->level * 100;
		GetInfo()->hp = GetInfo()->level * 70;
	}
	send_stat_change();
	c_lock.unlock();
}

void CClient::HitByPlayer(char* mess)
{
	c_lock.lock();
	GetInfo()->hp -= 100;
	c_lock.unlock();
}

void CClient::send_login_fail()
{
	sc_packet_login_fail p;
	p.id = GetInfo()->id;
	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	strcpy_s(p.message, "another client is using this name");
	send_packet(&p);
}

void CClient::send_login_ok()
{
	sc_packet_login_ok p;
	p.exp = GetInfo()->exp;
	p.hp = GetInfo()->hp;
	p.id = GetInfo()->id;
	p.level = GetInfo()->level;
	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	p.x = GetInfo()->x;
	p.y = GetInfo()->y;
	send_packet(&p);
}

void CClient::send_heal_packet(char* mess)
{
	sc_packet_stat_change p;
	p.level = GetInfo()->level;
	p.exp = GetInfo()->exp;
	p.hp = GetInfo()->hp;
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

void CClient::send_enter_packet(CCharacter* other)
{
	sc_packet_enter p;
	p.id = other->GetInfo()->id;
	p.size = sizeof(p);
	p.type = SC_PACKET_ENTER;
	p.x = other->GetInfo()->x;
	p.y = other->GetInfo()->y;
	//if (p.id > MAX_USER)
	//{
	//	strcpy_s(p.name, other->GetInfo()->name.c_str());
	//}
	//else
	{
		strcpy_s(p.name, MAX_ID_LEN, other->GetInfo()->name.c_str());
	}
	p.o_type = 0;
	send_packet(&p);
}

void CClient::send_move_packet(CCharacter* other)
{
	sc_packet_move p;
	p.id = other->GetInfo()->id;
	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE;
	p.x = other->GetInfo()->x;
	p.y = other->GetInfo()->y;
	p.move_time = other->GetInfo()->move_time;
	send_packet(&p);
}

void CClient::send_stat_change()
{
	sc_packet_stat_change p;
	p.level = GetInfo()->level;
	p.exp = GetInfo()->exp;
	p.hp = GetInfo()->hp;
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

void CClient::StartRecv()
{
	DWORD flags = 0;
	int ret;
	this->c_lock.lock();
	ret = WSARecv(this->m_sock, &this->m_recv_over.wsa_buf, 1, NULL,
		&flags, &this->m_recv_over.wsa_over, NULL);
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
	viewLock.lock();
	viewList.erase(id);
	viewLock.unlock();
	send_leave_packet(id);
}

void CClient::EnterPlayer(CCharacter* other)
{
	viewLock.lock();
	viewList.insert(other->GetInfo()->id);
	send_enter_packet(other);
	viewLock.unlock();
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
	WSARecv(m_sock, &m_recv_over.wsa_buf,
		1, NULL, &recv_flag, &m_recv_over.wsa_over, NULL);
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
	WSASend(this->m_sock, &send_over->wsa_buf, 1,
		NULL, 0, &send_over->wsa_over, NULL);
	this->c_lock.unlock();
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
