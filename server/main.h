#pragma once

#include <windows.h>

#pragma pack(1)

typedef struct
{
	DWORD ip;
	DWORD hbi;
	DWORD fzLimit;
	DWORD bHttps;
	char path[MAX_PATH];
}PROGRAM_PARAMS;

#pragma pack()

int init();

int clear();