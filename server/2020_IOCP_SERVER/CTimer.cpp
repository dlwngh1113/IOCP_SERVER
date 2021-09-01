#include "CTimer.h"

CTimer::CTimer()
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
					//random_move_npc(ev.obj_id);
					OVER_EX* over_ex = new OVER_EX;
					over_ex->op_mode = OP_RANDOM_MOVE;
					PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &over_ex->wsa_over);
					//add_timer(ev.obj_id, OP_RANDOM_MOVE, system_clock::now() + 1s);
				}
				break;
				case OP_RUNAWAY:
				{
					send_chat_packet(ev.target_id, ev.obj_id, ev.message);
				}
				break;
				case OP_REVIVAL:
				{
					g_clients[ev.obj_id].level = rand() % 10 + 1;
					g_clients[ev.obj_id].hp = g_clients[ev.obj_id].level * 100;
					for (int i = 0; i < MAX_USER; ++i) {
						if (is_near(ev.obj_id, i) && g_clients[i].in_use) {
							g_clients[i].vl.lock();
							g_clients[i].view_list.insert(ev.obj_id);
							send_enter_packet(i, ev.obj_id);
							g_clients[i].vl.unlock();
						}
					}
					OVER_EX* over_ex = new OVER_EX;
					over_ex->op_mode = OP_RANDOM_MOVE;
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

bool CTimer::is_near(int p1, int p2)
{
	int dist = (g_clients[p1].x - g_clients[p2].x) * (g_clients[p1].x - g_clients[p2].x);
	dist += (g_clients[p1].y - g_clients[p2].y) * (g_clients[p1].y - g_clients[p2].y);

	return dist <= VIEW_LIMIT * VIEW_LIMIT;
}

void CTimer::send_chat_packet(int to_client, int id, char* mess)
{
	sc_packet_chat p;
	p.id = id;
	p.size = sizeof(p);
	p.type = SC_PACKET_CHAT;
	strcpy_s(p.message, mess);
	send_packet(to_client, &p);
}

void CTimer::send_enter_packet(int to_client, int new_id)
{
	sc_packet_enter p;
	p.id = new_id;
	p.size = sizeof(p);
	p.type = SC_PACKET_ENTER;
	p.x = g_clients[new_id].x;
	p.y = g_clients[new_id].y;
	g_clients[new_id].c_lock.lock();
	strcpy_s(p.name, g_clients[new_id].name);
	g_clients[new_id].c_lock.unlock();
	p.o_type = 0;
	send_packet(to_client, &p);
}

void CTimer::join()
{
	std::thread timer_thread{ time_worker };
}
