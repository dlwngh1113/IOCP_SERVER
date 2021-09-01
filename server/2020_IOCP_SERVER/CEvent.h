#pragma once
#include<chrono>
class CEvent
{
public:
	int obj_id;
	std::chrono::system_clock::time_point wakeup_time;
	int event_id;
	int target_id;
	char* message;

	constexpr bool operator < (const CEvent& _Left) const
	{
		return (wakeup_time > _Left.wakeup_time);
	}
};

