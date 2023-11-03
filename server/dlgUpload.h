
#pragma once

#include <windows.h>
#include <iostream>

using namespace std;


#pragma pack(1)


#pragma pack()

class DialogUpload {
public:
	DialogUpload();
	DialogUpload(string id);
	~DialogUpload();

	static int __stdcall runDlgUpload(DialogUpload* dialog);

	static INT_PTR dlgUploadProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hwnd;
	HINSTANCE m_hinst;
	string m_id;
};


int createDlgUpload(HINSTANCE, string id);