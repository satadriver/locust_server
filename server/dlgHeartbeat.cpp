
#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgHeartbeat.h"

#include "public.h"
#include "mission.h"
#include "packet.h"
#include "dlgCmd.h"

using namespace std;


DialogHeartbeat* g_dlgHB = 0;

DialogHeartbeat::DialogHeartbeat() {

}


DialogHeartbeat::DialogHeartbeat(string id) {
	m_id = id;
}

DialogHeartbeat::~DialogHeartbeat() {
	if (g_dlgHB)
	{
		delete g_dlgHB;
		g_dlgHB = 0;
	}
}


int __stdcall heartbeatRandom(CMD_PARAMS* params) {

	int ret = 0;

	string id = params->id;
	string cmd = params->cmd;

	delete params;

	PacketParcel packet(TRUE, id);

	char* buf = buildCmd((char*)cmd.c_str(), cmd.size(), COMMAND_TYPE_HEARTBEAT);

	ret = packet.postCmdFile(CMD_SEND_DD_DATA, buf, cmd.size() + sizeof(MY_CMD_PACKET));

	delete buf;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4) != DATA_PACK_TAG)
	{
		return FALSE;
	}

	return 0;
}


INT_PTR DialogHeartbeat::dlgHBProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_INITDIALOG)
	{

	}
	else if ((msg & 0xffff) == WM_COMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
		if (wparam == IDOK)
		{
			char szcmd[0x1000];
			int len = GetDlgItemTextA(g_dlgHB->m_hwnd, IDC_EDIT3, szcmd, sizeof(szcmd));
			if (len > 0)
			{
				CMD_PARAMS* params = new CMD_PARAMS;
				params->id = string(g_dlgHB->m_id);
				params->cmd = string(szcmd);
				//int ret = runcmd(&params);

				HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)heartbeatRandom, params, 0, 0);
				if (ht)
				{
					CloseHandle(ht);
					EndDialog(g_dlgHB->m_hwnd,0);
				}
			}
		}
	}
	else if (msg == WM_SYSCOMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
	}
	else if (msg == WM_CTLCOLORDLG)
	{

	}
	else if (msg == WM_CLOSE)
	{
		EndDialog(hwnd, 0);
	}
	else if (msg == WM_DESTROY)
	{
		EndDialog(hwnd, 0);
	}
	else if (msg == WM_CHAR)
	{
		printf("hello");
	}
	else if (msg == WM_RBUTTONDOWN)
	{

	}
	else if (msg == WM_LBUTTONDOWN)
	{

	}
	return 0;
}



int __stdcall DialogHeartbeat::runDlgHB(DialogHeartbeat* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG3, 0, (DLGPROC)dlgHBProc, 0);
	if (dialog->m_hwnd == 0)
	{
		ret = GetLastError();
		return FALSE;
	}

	ret = ShowWindow(dialog->m_hwnd, SW_SHOW);

	MSG msg;
	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {

		if (ret == -1) {
			return -1;
		}

		if (!IsDialogMessage(dialog->m_hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}


int createDlgHB(HINSTANCE hinst, string id) {
	g_dlgHB = new DialogHeartbeat(id);
	g_dlgHB->m_hinst = hinst;
	//g_dlgCmd->runDlgCmd(g_dlgCmd);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgHB->runDlgHB, g_dlgHB, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}