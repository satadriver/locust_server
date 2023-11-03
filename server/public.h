#pragma once

#include <windows.h>



#define UUID_LENGTH				32

#define MY_USERAGENT			L"myUserAgent"

#define MY_MUTEX_NAME			"MY_MUTEX_NAME"

extern "C" DWORD g_ip;

extern "C" DWORD g_httpsToggle;

extern "C" char g_uuid[64];

extern "C" int g_uuid_len;

extern "C" int g_interval;