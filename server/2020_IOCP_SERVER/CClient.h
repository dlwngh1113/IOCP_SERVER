#pragma once
#include"CCharacter.h"

class CClient : public CCharacter
{
	SOCKET	m_sock;
	OVER_EX	m_recv_over;
	unsigned char* m_packet_start;
	unsigned char* m_recv_start;

	int move_time;
	int atk_time;

	void send_packet(void* p);
public:
#pragma region getter
	unsigned char* getPacketStart();
	unsigned char* getRecvStart();
	char getPacketType() const;
#pragma endregion

#pragma region setter
	void SetInfo(const char* name, short level, short x, short y, int exp, short hp);
	virtual void Teleport(short x, short y);
#pragma endregion

#pragma region	ref
	int& getAtktime();
	int& getMoveTime();
#pragma endregion
	CClient();
	CClient(int id, std::string name, short x, short y, SOCKET s);
	CClient(CInfo* info);
	virtual ~CClient();

	void Release();

	void AutoHeal();
	void LevelUp(int targetID, int exp);
	void HitByPlayer(char* mess);
	void StartRecv();
	void ErasePlayer(int id);
	void EnterPlayer(CCharacter* other);

	void IncreaseBuffer(DWORD iosize, long long left_data);

	void send_login_fail();
	void send_login_ok();
	void send_heal_packet(char* mess);
	void send_leave_packet(int targetID);
	void send_enter_packet(CCharacter* other);
	void send_move_packet(CClient* other);
	void send_stat_change();
	void send_chat_packet(int targetID, char* mess);
};