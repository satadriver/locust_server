#pragma once


#pragma once



#pragma once

#include <windows.h>
#include <iostream>

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


int createDlgOnline(HINSTANCE, string id);