//#include "CNPCController.h"
//
//CNPCController::CNPCController()
//{
//}
//
//CNPCController::~CNPCController()
//{
//}
//
//void CNPCController::npc_ai_thread()
//{
//	while (true) {
//		auto start_time = std::chrono::system_clock::now();
//		for (int i = MAX_USER; i < MAX_USER + NUM_NPC; ++i)
//			if (g_clients[i].hp > 0)
//				random_move_npc(i);
//		auto end_time = std::chrono::system_clock::now();
//		auto exec_time = end_time - start_time;
//		std::cout << "AI exec time = " << std::chrono::duration_cast<std::chrono::seconds>(exec_time).count() << "s\n";
//		std::this_thread::sleep_for(std::chrono::seconds(1) - (end_time - start_time));
//	}
//}
//
//void CNPCController::random_move_npc(int id)
//{
//	std::unordered_set <int> old_viewlist;
//	for (int i = 0; i < MAX_USER; ++i) {
//		if (false == g_clients[i].in_use) continue;
//		if (true == is_near(id, i)) old_viewlist.insert(i);
//	}
//	int x = g_clients[id].x;
//	int y = g_clients[id].y;
//	switch (rand() % 4)
//	{
//	case 0: if (x > 0) x--; break;
//	case 1: if (x < (WORLD_WIDTH - 1)) x++; break;
//	case 2: if (y > 0) y--; break;
//	case 3: if (y < (WORLD_HEIGHT - 1)) y++; break;
//	}
//	g_clients[id].x = x;
//	g_clients[id].y = y;
//	std::unordered_set <int> new_viewlist;
//	for (int i = 0; i < MAX_USER; ++i) {
//		if (id == i) continue;
//		if (false == g_clients[i].in_use) continue;
//		if (true == is_near(id, i)) new_viewlist.insert(i);
//	}
//
//	for (auto pl : old_viewlist) {
//		if (0 < new_viewlist.count(pl)) {
//			if (0 < g_clients[pl].view_list.count(id))
//				send_move_packet(pl, id);
//			else {
//				g_clients[pl].view_list.insert(id);
//				send_enter_packet(pl, id);
//			}
//		}
//		else
//		{
//			if (0 < g_clients[pl].view_list.count(id)) {
//				g_clients[pl].view_list.erase(id);
//				g_clients[pl].send_leave_packet(id);
//			}
//		}
//	}
//
//	for (auto pl : new_viewlist) {
//		if (0 == g_clients[pl].view_list.count(pl)) {
//			if (0 == g_clients[pl].view_list.count(id)) {
//				g_clients[pl].view_list.insert(id);
//				send_enter_packet(pl, id);
//			}
//			else
//				send_move_packet(pl, id);
//		}
//	}
//
//	if (true == new_viewlist.empty()) {
//		g_clients[id].is_active = false;
//	}
//	else {
//		add_timer(id, OP_RANDOM_MOVE, std::chrono::system_clock::now() + std::chrono::seconds(1));
//	}
//
//	for (auto pc : new_viewlist) {
//		OVER_EX* over_ex = new OVER_EX;
//		over_ex->object_id = pc;
//		over_ex->op_mode = OP_PLAYER_MOVE_NOTIFY;
//		PostQueuedCompletionStatus(h_iocp, 1, id, &over_ex->wsa_over);
//	}
//}
