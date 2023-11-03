
#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgCmd.h"

#include "public.h"
#include "mission.h"
#include "packet.h"

using namespace std;


DialogCommand * g_dlgCmd = 0;

DialogCommand::DialogCommand() {

}


DialogCommand::DialogCommand(string id) {
	m_id = id;
}

DialogCommand::~DialogCommand() {
	if (g_dlgCmd)
	{
		delete g_dlgCmd;
		g_dlgCmd = 0;
	}
}


int __stdcall runcmd(CMD_PARAMS *params) {

	int ret = 0;

	PacketParcel packet(TRUE, params->id);

	char* buffer = buildCmd(params->cmd.c_str(), params->cmd.size(), MISSION_TYPE_CMD);

	ret = packet.postCmd(CMD_SEND_CMD, (char*)buffer, params->cmd.size() + sizeof(MY_CMD_PACKET));

	delete buffer;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();

	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4) != DATA_PACK_TAG)
	{
		return FALSE;
	}

	while (TRUE)
	{
		Sleep(g_interval);

		ret = packet.postCmd(CMD_GET_CMD_RESULT, 0, 0);

		data = packet.getbuf();
		datasize = packet.getbufsize();
		if (datasize < 4 || memcmp(data , CMD_SEND_CMD_RESULT,4 ) != 0)
		{
			continue;
		}
		else {
			break;
		}
	}

	MY_CMD_PACKET* inpack = (MY_CMD_PACKET*)(data + 4);
	int reslen = inpack->len;
	char* resdata = data + 4 + sizeof(MY_CMD_PACKET);
	*(resdata + reslen) = 0;
	
	int len = SetDlgItemTextA(g_dlgCmd->m_hwnd, IDC_EDIT2, resdata);

	delete params;

	return 0;
}


INT_PTR DialogCommand::dlgCmdProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
			int len = GetDlgItemTextA(g_dlgCmd->m_hwnd, IDC_EDIT1, szcmd, sizeof(szcmd));
			if (len > 0)
			{
				CMD_PARAMS *params = new CMD_PARAMS;
				params->id = string(g_dlgCmd->m_id);
				params->cmd = string(szcmd);
				//int ret = runcmd(&params);

				HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)runcmd, params, 0,0);
				if (ht)
				{
					CloseHandle(ht);
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



int __stdcall DialogCommand::runDlgCmd(DialogCommand * dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG1, 0, (DLGPROC)dlgCmdProc, 0);
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


int createDlgCmd(HINSTANCE hinst,string id) {
	g_dlgCmd = new DialogCommand(id);
	g_dlgCmd->m_hinst = hinst;
	//g_dlgCmd->runDlgCmd(g_dlgCmd);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgCmd->runDlgCmd, g_dlgCmd, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}