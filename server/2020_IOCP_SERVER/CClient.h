#pragma once
#include"framework.h"

class CClient
{
	std::mutex c_lock;
	char name[MAX_ID_LEN];
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
public:
	void SetUse(bool b);
};