#pragma once
#include<WinSock2.h>
#include<ws2def.h>

constexpr int SERVER_PORT = 3500;
constexpr int MAX_ID_LEN = 10;
constexpr int MAX_USER = 10000;
constexpr int WORLD_WIDTH = 800;
constexpr int WORLD_HEIGHT = 800;
constexpr int MAX_STR_LEN = 100;
constexpr int VIEW_LIMIT = 7;				// 시야 반지름, 상대방과 사이에 6개의 타일이 있어도 보여야 함.

constexpr int NUM_NPC = 10'000;

constexpr int MAX_BUFFER = 4096;
constexpr int MIN_BUFF_SIZE = 1024;

constexpr char OP_MODE_RECV = 0;
constexpr char OP_MODE_SEND = 1;
constexpr char OP_MODE_ACCEPT = 2;
constexpr char OP_RANDOM_MOVE = 3;
constexpr char OP_PLAYER_MOVE_NOTIFY = 4;
constexpr char OP_RUNAWAY = 5;
constexpr char OP_REVIVAL = 6;
constexpr char OP_HEAL = 7;

constexpr int  KEY_SERVER = 1000000;

constexpr char SC_PACKET_LOGIN_OK	= 0;
constexpr char SC_PACKET_MOVE		= 1;
constexpr char SC_PACKET_ENTER		= 2;
constexpr char SC_PACKET_LEAVE		= 3;
constexpr char SC_PACKET_CHAT		= 4;
constexpr char SC_PACKET_LOGIN_FAIL	= 5;
constexpr char SC_PACKET_STAT_CHANGE	= 6;

constexpr char CS_LOGIN		= 0;
constexpr char CS_MOVE		= 1;
constexpr char CS_ATTACK	= 2;
constexpr char CS_CHAT		= 3;
constexpr char CS_LOGOUT	= 4;
constexpr char CS_TELEPORT	= 5;				// 부하 테스트용 동접 테스트를 위해 텔러포트로 Hot Spot 해소

struct OVER_EX {
	WSAOVERLAPPED wsa_over;
	char	op_mode;
	WSABUF	wsa_buf;
	unsigned char iocp_buf[MAX_BUFFER];
	int		object_id;
};

#pragma pack (push, 1)

struct sc_packet_login_ok {
	char size;
	char type;
	int  id;
	short x, y;
	short hp;
	short level;
	int   exp;
};

struct sc_packet_move {
	char size;
	char type;
	int id;
	short x, y;
	int move_time;
};

struct sc_packet_enter {
	char size;
	char type;
	int  id;
	char name[MAX_ID_LEN];
	char o_type;
	short x, y;
};

struct sc_packet_leave {
	char size;
	char type;
	int  id;
};

struct sc_packet_chat {
	char  size;
	char  type;
	int	  id;			// teller
	char  message[MAX_STR_LEN];
};

struct sc_packet_login_fail {
	char  size;
	char  type;
	int	  id;			
	char  message[MAX_STR_LEN];
};

struct sc_packet_stat_change {
	char size;
	char type;
	int  id;
	short hp;
	short level;
	int   exp;
	char message[MAX_STR_LEN];
};


struct cs_packet_login {
	char  size;
	char  type;
	char  name[MAX_ID_LEN];
};

constexpr char MV_UP = 0;
constexpr char MV_DOWN = 1;
constexpr char MV_LEFT = 2;
constexpr char MV_RIGHT = 3;

struct cs_packet_move {
	char  size;
	char  type;
	char  direction;
	int	  move_time;
};

struct cs_packet_attack {
	char	size;
	char	type;
	int		atk_time;
};

struct cs_packet_chat {
	char	size;
	char	type;
	char	message[MAX_STR_LEN];
};

struct cs_packet_logout {
	char	size;
	char	type;
};

struct cs_packet_teleport {
	char size;
	char type;
	short x, y;
};

#pragma pack (pop)

