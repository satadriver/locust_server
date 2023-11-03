#pragma once

#include <windows.h>
#include <iostream>

using namespace std;


#pragma pack(1)

typedef struct {

	string id;
	string cmd;
	string append;
}CMD_PARAMS;
#pragma pack()

class DialogCommand {
public:
	DialogCommand();
	DialogCommand(string id);
	~DialogCommand();

	static int __stdcall runDlgCmd(DialogCommand* dialog);

	static INT_PTR dlgCmdProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hwnd;
	HINSTANCE m_hinst;
	string m_id;
};


int createDlgCmd(HINSTANCE,string id);