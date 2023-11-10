



#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgrename.h"

#include "public.h"
#include "mission.h"
#include "packet.h"
#include "dlgCmd.h"
#include "utils.h"

using namespace std;


DialogRename* g_dlgRename = 0;



DialogRename::DialogRename() {

}


DialogRename::DialogRename(string id,string path,string srcfn) {
	m_id = id;
	
	if (path.back() != '\\' && path.back() != '/')
	{
		path.append("\\");
	}
	m_path = path;
	m_srcfn = srcfn;

	//why can not use "g_dlgRename->m_srcfn = srcfn;" ?

}

DialogRename::~DialogRename() {
	if (g_dlgRename)
	{
// 		delete g_dlgRename;
// 		g_dlgRename = 0;
	}
}


int __stdcall dlgRenameFile(CMD_PARAMS* params) {

	int ret = 0;

	string id = params->id;
	string path = params->cmd;
	string newpath = params->append;

	delete params;

	PacketParcel packet(TRUE, id);

	char* buf = buildCmd2(path.c_str(), path.size(), MISSION_TYPE_RENFILE, newpath.c_str(), newpath.size());
	int packsize = sizeof(MY_CMD_PACKET) + sizeof(MY_CMD_PACKET) + newpath.size() + path.size();
	ret = packet.postCmdFile(CMD_BRING_COMMAND, buf, packsize);

	opLog("object:%s rename file:%s to file:%s\r\n", id.c_str(), path.c_str(),newpath.c_str());

	delete buf;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG )
	{
		return FALSE;
	}

	return TRUE;
}


INT_PTR DialogRename::dlgRenameProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
			int len = GetDlgItemTextA(g_dlgRename->m_hwnd, IDC_EDIT1, szcmd, sizeof(szcmd));
			if (len > 0)
			{
				CMD_PARAMS* params = new CMD_PARAMS;
				params->id = string(g_dlgRename->m_id);
				params->cmd = g_dlgRename->m_path + g_dlgRename-> m_srcfn;
				params->append = g_dlgRename->m_path + string(szcmd);
				HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)dlgRenameFile, params, 0, 0);
				if (ht)
				{
					CloseHandle(ht);
					EndDialog(g_dlgRename->m_hwnd, 0);
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
		DestroyWindow(hwnd);
	}
	else if (msg == WM_DESTROY)
	{
		PostQuitMessage(-1);
	}
	else if (msg == WM_CHAR)
	{

	}
	else if (msg == WM_RBUTTONDOWN)
	{

	}
	else if (msg == WM_LBUTTONDOWN)
	{

	}
	return 0;
}



int __stdcall DialogRename::runDlgRename(DialogRename* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG6, 0, (DLGPROC)dlgRenameProc, 0);
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

	if (g_dlgRename)
	{
		delete g_dlgRename;
		g_dlgRename = 0;
	}
	return 0;
}


int createDlgRename(HINSTANCE hinst, string id,string path,string srcfn) {
	g_dlgRename = new DialogRename(id,path,srcfn);
	g_dlgRename->m_hinst = hinst;


	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgRename->runDlgRename, g_dlgRename, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}