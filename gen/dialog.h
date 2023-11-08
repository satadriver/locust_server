#pragma once


#pragma once

#include <windows.h>

#include <iostream>

using namespace std;







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


#define  MAX_UPLOAD_FILESIZE			64

#define HEART_BEAT_INTERVAL				6

#define HEART_BEAT_TEST_INTERVAL		1


int createDialog(HINSTANCE inst);

class MyDialog {
public:
	MyDialog();

	~MyDialog();

	static int __stdcall runDialog(MyDialog* dialog);

	static INT_PTR dlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hwnd;

	HINSTANCE m_hinst;

protected:
};




