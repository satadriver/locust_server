#pragma once



#pragma once

#include <windows.h>
#include <iostream>

using namespace std;


#pragma pack(1)


#pragma pack()

class DialogHeartbeat {
public:
	DialogHeartbeat();
	DialogHeartbeat(string id);
	~DialogHeartbeat();

	static int __stdcall runDlgHB(DialogHeartbeat* dialog);

	static INT_PTR dlgHBProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hwnd;
	HINSTANCE m_hinst;
	string m_id;
};


int createDlgHB(HINSTANCE, string id);