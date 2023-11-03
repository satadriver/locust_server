#pragma once

#include <windows.h>

#include <iostream>

using namespace std;

#define CMD_UPDATE_HOSTS	WM_USER+1

#define HOST_HEARTBEAT_INTERVAL 30000


#define ONLINE_KEYNAME_ID		"ID"
#define ONLINE_KEYNAME_IP		"IP"
#define ONLINE_KEYNAME_TIME		"TIME"
#define ONLINE_KEYNAME_STATUS	"STATUS"

#define ONLINE_FORAMT						"ID:%s,IP:%s,TIME:%s,STATUS:%s"

#define ONLINE_KEYVALUE_STATUS_ALIVE		"alive"

#define ONLINE_KEYVALUE_STATUS_DEAD			"dead"

#pragma pack(1)

typedef struct  
{
	string id;
	string ip;
	string status;
	string time;
	
	int num;
	int valid;
}ONLINE_INFO;

#pragma pack()



int createDialog(HINSTANCE inst);

class MyDialog {
public:
	MyDialog();

	~MyDialog();

	static int __stdcall ruDialog(MyDialog *dialog);

	static INT_PTR dlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	int showObjects();

	int rmenu();

	int menu();

	string getFeild(const char* str, const char* key);

	HWND m_hwnd;

	HINSTANCE m_hinst;

	WNDPROC m_listboxProc;

	//int (* m_listboxProc)(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	int m_clock;
protected:
};




