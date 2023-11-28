#pragma once

#include <windows.h>

#include <iostream>

using namespace std;

#define CMD_UPDATE_HOSTS			WM_USER +	0x1001
#define CMD_UPDATE_DRIVE			WM_USER	+	0x1000

#define HOST_HEARTBEAT_INTERVAL		6000


#define ONLINE_KEYNAME_ID		"ID"
#define ONLINE_KEYNAME_IP		"IP"
#define ONLINE_KEYNAME_TIME		"TIME"
#define ONLINE_KEYNAME_STATUS	"STATUS"

#define OBJECT_INFO_FORAMT					"ID:%s,IP:%s,TIME:%s"
#define ONLINE_FORAMT						OBJECT_INFO_FORAMT ",STATUS:%s\r\n"

#define ONLINE_KEYVALUE_STATUS_ALIVE		"ALIVE"

#define ONLINE_KEYVALUE_STATUS_DEAD			"DEAD"

#define LOCAL_OBJECTS_FILENAME				"sysinfo"



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

	static int __stdcall runDialog(MyDialog *dialog);

	static INT_PTR dlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	int updateObjects();

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




