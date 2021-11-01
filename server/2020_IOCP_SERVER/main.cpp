#include"CServer.h"

CTimer* CTimer::timer = nullptr;
std::unordered_map<int, CCharacter*> CServer::characters = std::unordered_map<int, CCharacter*>();

int main()
{
	CServer server;
	server.run();
}