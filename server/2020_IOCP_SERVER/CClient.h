#pragma once
#include"CMonster.h"

class CClient : public CCharacter
{
	std::mutex c_lock;
	short level;
	short hp;
	int exp;

	SOCKET	m_sock;
	OVER_EX	m_recv_over;
	unsigned char* m_packet_start;
	unsigned char* m_recv_start;

	int move_time;
	int atk_time;

	void send_packet(void* p);
public:
#pragma region getter
	virtual std::string& GetName();
	virtual std::unordered_set<int>& GetViewlist();
	virtual short GetX() const;
	virtual short GetY() const;
	virtual std::mutex& GetViewlock();
	short getHP() const;
	short getLevel() const;
	int getExp() const;
	unsigned char* getPacketStart();
	unsigned char* getRecvStart();
	char getPacketType() const;
#pragma endregion

#pragma region setter
	void SetClient(int id, SOCKET ns);
	void SetInfo(char* name, short level, short x, short y, int exp, short hp);
	virtual void Teleport(short x, short y);
#pragma endregion

#pragma region	ref
	int& getAtktime();
	int& getMoveTime();
#pragma endregion
	CClient();
	virtual ~CClient();

	void Init(short x, short y, short level, char* name, int i);
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