#pragma once

#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <SDKDDKVer.h>
#include <vector>
#include <atlimage.h>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "Resource.h"

constexpr int MAX_LOADSTRING = 100;
constexpr int SCREEN_WIDTH = 1080;
constexpr int SCREEN_HEIGHT = 720;
