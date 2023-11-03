


#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgUpload.h"

#include "public.h"
#include "mission.h"
#include "packet.h"
#include "dlgCmd.h"

using namespace std;


DialogUpload* g_dlgUpload = 0;

DialogUpload::DialogUpload() {

}


DialogUpload::DialogUpload(string id) {
	m_id = id;
}

DialogUpload::~DialogUpload() {
	if (g_dlgUpload)
	{
		delete g_dlgUpload;
		g_dlgUpload = 0;
	}
}


int __stdcall uploadFile(CMD_PARAMS* params) {
// 	CMD_PARAMS* params = new CMD_PARAMS;
// 	params->id = dialog->m_id;
// 	params->cmd = "";
// 	HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)onlineInfo, params, 0, 0);
// 	if (ht)
// 	{
// 		CloseHandle(ht);
// 	}

	int ret = 0;

	string id = params->id;
	string cmd = params->cmd;

	delete params;

	PacketParcel packet(TRUE, id);

	ret = packet.postCmd(CMD_READ_ONLINE, 0, 0);

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4) != DATA_PACK_TAG)
	{
		return FALSE;
	}

	string stronline = string(data + 4, datasize - 8);
	HWND list = GetDlgItem(g_dlgUpload->m_hwnd, IDC_LIST3);
	ret = SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)stronline.c_str());
	return 0;
}


INT_PTR DialogUpload::dlgUploadProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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



int __stdcall DialogUpload::runDlgUpload(DialogUpload* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG5, 0, (DLGPROC)dlgUploadProc, 0);
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


int createDlgUpload(HINSTANCE hinst, string id) {
	g_dlgUpload = new DialogUpload(id);
	g_dlgUpload->m_hinst = hinst;
	//g_dlgCmd->runDlgCmd(g_dlgCmd);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgUpload->runDlgUpload, g_dlgUpload, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}