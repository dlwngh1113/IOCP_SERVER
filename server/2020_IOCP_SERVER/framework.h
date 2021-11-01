#pragma once

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <concurrent_unordered_map.h>
#include <chrono>
#include <queue>
#include <string>
#include <windows.h>  
#include <stdio.h>  
#include <sqlext.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "lua54.lib")
#pragma commint(lib, "odbc32")

#include"protocol.h"