#pragma once
#include"CEvent.h"

class CTimer
{
	static CTimer* timer;
	HANDLE h_iocp;
	std::priority_queue<CEvent> timer_queue;
	std::mutex timer_l;
public:
	static CTimer* GetInstance()
	{
		if (!timer)
			timer = new CTimer();
		return timer;
	}
	CTimer() = default;
	CTimer(HANDLE h_iocp);
	virtual ~CTimer();
	void SetHandle(HANDLE h_iocp) { this->h_iocp = h_iocp; }
	void add_timer(int obj_id, int ev_type, std::chrono::system_clock::time_point t, int target_id = NULL, char* mess = NULL);
	void time_worker();
	void join();
};

