#pragma once


#include <windows.h>

int getUUID();

int getWIndowsUUID(WCHAR* uuid, int size);

int getVolumeSerialNum(DWORD* lpulVolumeSerialNo);