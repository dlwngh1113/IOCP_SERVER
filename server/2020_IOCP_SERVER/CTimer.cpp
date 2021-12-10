#include "CTimer.h"
#include"CServer.h"

CTimer::CTimer(HANDLE h_iocp) : h_iocp{ h_iocp }
{
}

CTimer::~CTimer()
{
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
					reinterpret_cast<CClient*>(CServer::characters[ev.target_id])->send_chat_packet(ev.obj_id, ev.message);
				}
				break;
				case OP_REVIVAL:
				{
					auto player = reinterpret_cast<CClient*>(CServer::characters[ev.obj_id]);
					player->GetInfo()->level = rand() % 10 + 1;
					player->GetInfo()->hp = player->GetInfo()->level * 100;
					for (auto i = CServer::characters.begin(); i != CServer::characters.end(); ++i)
					{
						if (CServer::is_near(ev.obj_id, i->first) && i->first < MAX_USER)
							player->EnterPlayer(CServer::characters[i->first]);
					}

					OVER_EX* over_ex = new OVER_EX;
					over_ex->op_mode = OP_REVIVAL;
					PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &over_ex->wsa_over);
				}
				break;
				case OP_HEAL:
				{
					//auto maxHp = CServer::characters[ev.obj_id]->GetInfo()->level * 70;
					//if (CServer::characters[ev.obj_id]->GetInfo()->hp != maxHp)
					{
						OVER_EX* over_ex = new OVER_EX;
						over_ex->op_mode = OP_HEAL;
						PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &over_ex->wsa_over);
					}
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

void CTimer::join()
{
	std::thread timer_thread([&]() {time_worker(); });
	timer_thread.join();
}
