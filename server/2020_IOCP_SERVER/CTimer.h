#pragma once
#include"framework.h"
#include"CEvent.h"
class CTimer
{
	std::priority_queue<CEvent> timer_queue;
	std::mutex timer_l;
public:
	CTimer();
	virtual ~CTimer();
	void add_timer(int obj_id, int ev_type, std::chrono::system_clock::time_point t, int target_id = NULL, char* mess = NULL);
	void time_worker();
	bool is_near(int p1, int p2);
	void send_chat_packet(int to_client, int id, char* mess);
	void send_enter_packet(int to_client, int new_id);
	void join();
};

