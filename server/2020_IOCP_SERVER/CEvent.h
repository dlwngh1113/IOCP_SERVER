#pragma once
#include"framework.h"
class CEvent
{
public:
	int obj_id;
	std::chrono::system_clock::time_point wakeup_time;
	int event_id;
	int target_id;
	char* message;
	CEvent(int obj_id, std::chrono::system_clock::time_point t, int event_id, int target_id, char* message):
		obj_id{ obj_id }, wakeup_time{ t }, event_id{ event_id }, target_id{ target_id }, message{ message }
	{
	}

	constexpr bool operator < (const CEvent& _Left) const
	{
		return (wakeup_time > _Left.wakeup_time);
	}
};

