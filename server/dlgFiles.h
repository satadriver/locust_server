#pragma once

#include <windows.h>
#include <iostream>

using namespace std;







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

	string setPath(string path);

	string getPath();

	int rmenu();


	HWND m_hwnd;

	HINSTANCE m_hinst;

	string m_id;

	string m_dir;

	WNDPROC m_lbProc;
};

int createDlgFile(HINSTANCE hinst, string id);