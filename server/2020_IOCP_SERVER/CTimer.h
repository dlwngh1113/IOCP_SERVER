#pragma once
#include"CEvent.h"

class CTimer
{
	HANDLE h_iocp;
	std::priority_queue<CEvent> timer_queue;
	std::mutex timer_l;
public:
	CTimer() = default;
	CTimer(HANDLE h_iocp);
	virtual ~CTimer();
	void add_timer(int obj_id, int ev_type, std::chrono::system_clock::time_point t, int target_id = NULL, char* mess = NULL);
	void time_worker();
	void send_chat_packet(int to_client, int id, char* mess);
	void join();
};

