#pragma once


#pragma once



#pragma once

#include <windows.h>
#include <iostream>
#include <vector>
#include "dlgCmd.h"
#include "command.h"

using namespace std;


#pragma pack(1)


#pragma pack()

class DialogOnline {
public:
	DialogOnline();
	DialogOnline(string id);
	~DialogOnline();

	static int __stdcall runDlgOnline(DialogOnline* dialog);

	static INT_PTR dlgOnlineProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hwnd;
	HINSTANCE m_hinst;
	string m_id;
};

int __stdcall onlineInfo(CMD_PARAMS* params);

int createDlgOnline(HINSTANCE, string id);

vector<CLIENT_INFO> getOnlineInfo(const char* data, int size);