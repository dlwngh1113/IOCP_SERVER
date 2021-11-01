#include "CTimer.h"

CTimer::CTimer(HANDLE h_iocp) : h_iocp{ h_iocp }
{
}

CTimer::~CTimer()
{
}

void CTimer::SetHandle(HANDLE h_iocp)
{
	this->h_iocp = h_iocp;
}

void CTimer::add_timer(int obj_id, int ev_type, std::chrono::system_clock::time_point t, int target_id, char* mess)
{
	CEvent ev{ obj_id, t, ev_type, target_id, mess };
	timer_l.lock();
	timer_queue.push(ev);
	timer_l.unlock();
}

void CTimer::time_worker()
{
	while (true) {
		while (true) {
			if (false == timer_queue.empty()) {
				timer_l.lock();
				CEvent ev = timer_queue.top();
				timer_l.unlock();
				if (ev.wakeup_time > std::chrono::system_clock::now())
					break;
				timer_l.lock();
				timer_queue.pop();
				timer_l.unlock();

				switch (ev.event_id)
				{
				case OP_RANDOM_MOVE:
				{
					OVER_EX* over_ex = new OVER_EX;
					over_ex->op_mode = OP_RANDOM_MOVE;
					PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &over_ex->wsa_over);
				}
				break;
				case OP_RUNAWAY:
				{
					send_chat_packet(ev.target_id, ev.obj_id, ev.message);
				}
				break;
				case OP_REVIVAL:
				{
					OVER_EX* over_ex = new OVER_EX;
					over_ex->op_mode = OP_REVIVAL;
					PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &over_ex->wsa_over);
				}
				break;
				case OP_HEAL:
				{
					OVER_EX* over_ex = new OVER_EX;
					over_ex->op_mode = OP_HEAL;
					PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &over_ex->wsa_over);
				}
				break;
				default:
					printf("Unknown event type: %c\n", ev.event_id);
					break;
				}
			}
			else break;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

void CTimer::send_chat_packet(int to_client, int id, char* mess)
{
	sc_packet_chat p;
	p.id = id;
	p.size = sizeof(p);
	p.type = SC_PACKET_CHAT;
	strcpy_s(p.message, mess);
	//send_packet(to_client, &p);
}

void CTimer::join()
{
	std::thread timer_thread([&]() {time_worker(); });
	timer_thread.join();
}
