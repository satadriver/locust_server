#pragma once

#include <windows.h>

#define HTTP_COMM_TYPE_FILE		1

#define UUID_LENGTH				32

#define MY_USERAGENT L"myUserAgent"

extern "C" DWORD g_ip;

extern "C" DWORD g_httpsToggle;

extern "C" char g_uuid[64];

extern "C" int g_uuid_len;