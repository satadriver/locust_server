#pragma once

#include <windows.h>
#include <iostream>

using namespace std;


#define CMD_UPDATE_DRIVE	WM_USER+2


#pragma pack(1)

typedef struct
{
	int type;
	ULONGLONG size;
	FILETIME createtime;
	FILETIME accesstime;
	FILETIME modifytime;

	int fnlen;
	char filename[MAX_PATH];

}FILE_INFOMATION;

#pragma pack()


class DialogFiles {
public:
	DialogFiles();

	~DialogFiles();

	DialogFiles(string id);

	static int __stdcall runDlgFile(DialogFiles* dialog);

	static INT_PTR dlgFileProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	string descent (string dir);

	string ascent();


	HWND m_hwnd;

	HINSTANCE m_hinst;

	string m_id;

	string m_dir;

};

int createDlgFile(HINSTANCE hinst, string id);