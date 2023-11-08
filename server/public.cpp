
#include <windows.h>

#include "public.h"

DWORD g_ip = 0;

DWORD g_httpsToggle = TRUE;

char g_uuid[64] = { 0 };

int g_uuid_len = 0;

int g_interval = 100;

HANDLE g_mutex_handle = 0;

int g_fsize_limit = 0;