#pragma once

#include <windows.h>
#include <iostream>

using namespace std;


#pragma pack(1)


#pragma pack()

class DialogRename{
public:
	DialogRename();
	DialogRename(string id,string path, string srcfn);
	~DialogRename();

	static int __stdcall runDlgRename(DialogRename* dialog);

	static INT_PTR dlgRenameProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hwnd;
	HINSTANCE m_hinst;
	string m_id;
	string m_srcfn;
	string m_path;
};


int createDlgRename(HINSTANCE, string id,string path,string srcfn);