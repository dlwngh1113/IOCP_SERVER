#pragma once
#include"framework.h"

class CClient
{
	std::mutex c_lock;
	int id;
	char* name;
	short level;
	short hp;
	short x, y;
	int exp;
	lua_State* L;
	std::mutex lua_l;

	bool in_use;
	std::atomic_bool is_active;
	SOCKET	m_sock;
	OVER_EX	m_recv_over;
	unsigned char* m_packet_start;
	unsigned char* m_recv_start;

	std::mutex vl;
	std::unordered_set <int> view_list;

	int move_time;
	int atk_time;

	void send_packet(void* p);
public:
#pragma region getter
	short getHP() const;
	bool getUse() const;
	char* getName();
	short getLevel() const;
	int getExp() const;
	unsigned char* getPacketStart();
	unsigned char* getRecvStart();
	char getPacketType() const;
	short getX() const;
	short getY() const;
#pragma endregion

#pragma region setter
	void SetUse(bool b);
	void SetClient(int id, SOCKET ns);
	void SetInfo(char* name, short level, short x, short y, int exp, short hp);
	void SetPosition(short x, short y);
#pragma endregion

#pragma region	ref
	std::unordered_set<int>& getViewList();
	int& getAtktime();
	int& getMoveTime();
	lua_State* getLua();
#pragma endregion
	CClient();
	virtual ~CClient();

	void Init(short x, short y, short level, char* name, int i);
	void Release();

	void MoveNotify(int objID);
	void AutoHeal();
	void LevelUp(int targetID, int exp);
	void HitByPlayer(char* mess);
	void StartRecv();
	void ErasePlayer(int id);
	void EnterPlayer(CClient& other);

	void IncreaseBuffer(DWORD iosize, long long left_data);

	void send_login_fail();
	void send_login_ok();
	void send_heal_packet(char* mess);
	void send_leave_packet(int targetID);
	void send_enter_packet(CClient& other);
	void send_move_packet(CClient& other);
	void send_stat_change();
	void send_chat_packet(int targetID, char* mess);

	bool CompareExchangeStrong(bool b);
};