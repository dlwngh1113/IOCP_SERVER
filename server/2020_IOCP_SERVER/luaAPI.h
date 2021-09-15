#pragma once

int API_get_x(lua_State* L)
{
	int user_id = lua_tointeger(L, -1);
	lua_pop(L, 2);
	int x = g_clients[user_id].x;
	lua_pushnumber(L, x);
	return 1;
}

int API_get_y(lua_State* L)
{
	int user_id = lua_tointeger(L, -1);
	lua_pop(L, 2);
	int y = g_clients[user_id].y;
	lua_pushnumber(L, y);
	return 1;
}

int API_SendEnterMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	lua_pop(L, 3);

	if (system_clock::now().time_since_epoch().count() > g_clients[my_id].atk_time)
	{
		g_clients[my_id].atk_time = system_clock::now().time_since_epoch().count();
		g_clients[user_id].hp -= 10;

		if (g_clients[user_id].hp <= 0) {
			g_clients[user_id].exp /= 2;
			g_clients[user_id].hp = g_clients[user_id].level * 70;
			g_clients[user_id].x = 0;
			g_clients[user_id].y = 0;
		}

		send_stat_change(user_id);
		char tmp[MAX_STR_LEN];
		sprintf_s(tmp, "You hit by id - %s", g_clients[my_id].name);
		send_chat_packet(user_id, user_id, tmp);
	}

	send_chat_packet(user_id, my_id, mess);
	return 0;
}

int API_SendLeaveMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	lua_pop(L, 3);

	add_timer(my_id, OP_RUNAWAY, system_clock::now() + 3s, user_id, mess);

	return 0;
}