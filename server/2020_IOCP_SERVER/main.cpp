#include"CServer.h"

CTimer* CTimer::timer = nullptr;
concurrency::concurrent_unordered_map<int, CCharacter*> CServer::characters = concurrency::concurrent_unordered_map<int, CCharacter*>();

int main()
{
	CServer server;
	server.run();
}