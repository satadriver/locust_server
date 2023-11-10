


#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgUpload.h"

#include "public.h"
#include "mission.h"
#include "packet.h"
#include "dlgCmd.h"
#include "FileHelper.h"
#include "utils.h"

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
// 		delete g_dlgUpload;
// 		g_dlgUpload = 0;
	}
}


string selectFileName() {
	int ret = 0;
	OPENFILENAMEA opfn = { 0 };
	CHAR file_name[0x1000];
	opfn.lStructSize = sizeof(OPENFILENAMEA);
	opfn.lpstrFilter = "所有文件\0*.*\0\0";
	//指向一对以空字符结束的过滤字符串的一个缓冲。缓冲中的最后一个字符串必须以两个  NULL字符结束。
	opfn.nFilterIndex = 1;    //指定在文件类型控件中当前选择的过滤器的索引
	opfn.lpstrFile = file_name;
	opfn.lpstrFile[0] = '\0'; //这个缓冲的第一个字符必须是NULL
	opfn.nMaxFile = sizeof(file_name);
	opfn.Flags = 0;  //OFN_FILEMUSTEXIST OFN_PATHMUSTEXIST指定用户仅可以在文件名登录字段中输入已存在的文件的名字。	
	ret = GetOpenFileNameA(&opfn);
	if (ret)
	{
		return string(file_name);
	}
	return "";
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
	string lfn = params->cmd;
	string rfn = params->append;

	delete params;

	PacketParcel packet(TRUE, id);

	char* filedata = 0;
	int filesize = 0;
	ret = FileHelper::fileReader(lfn.c_str(), &filedata, &filesize);
	if (ret == 0)
	{
		return FALSE;
	}
	char* buf = buildCmd2(rfn.c_str(),rfn.size(), MISSION_TYPE_UPLOAD, filedata, filesize);
	if (buf == 0)
	{
		return FALSE;
	}
	int packsize = sizeof(MY_CMD_PACKET) + sizeof(MY_CMD_PACKET) + filesize + rfn.size();
	ret = packet.postCmdFile(CMD_BRING_COMMAND, buf, packsize);
	delete buf;

	opLog("object:%s upload file:%s to file:%s\r\n", id.c_str(), lfn.c_str(),rfn.c_str());

	char* data = packet.getbuf();
	size_t datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG )
	{
		return FALSE;
	}

// 	string stronline = string(data + 4, datasize - 8);
// 	HWND list = GetDlgItem(g_dlgUpload->m_hwnd, IDC_LIST3);
// 	ret = SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)stronline.c_str());
	return TRUE;
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
		if (wl == IDC_BUTTON1)
		{
			string fn = selectFileName();
			if (fn != "")
			{
				SetDlgItemTextA(g_dlgUpload->m_hwnd, IDC_EDIT4, fn.c_str());
			}
		}
		else if (wl == IDC_BUTTON2)
		{
			string fn = selectFileName();
			if (fn != "")
			{
				SetDlgItemTextA(g_dlgUpload->m_hwnd, IDC_EDIT5, fn.c_str());
			}
		}
		else if (wl == IDOK)
		{
			int ret = 0;
			char lfn[0x1000];
			char rfn[0x1000];
			ret = GetDlgItemTextA(g_dlgUpload->m_hwnd, IDC_EDIT4, lfn, sizeof(lfn));
			ret = GetDlgItemTextA(g_dlgUpload->m_hwnd, IDC_EDIT5, rfn, sizeof(rfn));

			CMD_PARAMS* params = new CMD_PARAMS;
			params->id = g_dlgUpload->m_id;
			params->cmd = string(lfn);
			params->append = string(rfn);

			ret = uploadFile(params);
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

	if (g_dlgUpload)
	{
		delete g_dlgUpload;
		g_dlgUpload = 0;
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