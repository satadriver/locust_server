

#include "dlgfiles.h"

#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgCmd.h"

#include "public.h"
#include "mission.h"
#include "packet.h"
#include "FileHelper.h"

using namespace std;


DialogFiles* g_dlgFile = 0;

DialogFiles::DialogFiles() {

}


DialogFiles::DialogFiles(string id) {
	m_id = id;
	m_dir = "";
}

DialogFiles::~DialogFiles() {
	if (g_dlgFile)
	{
		delete g_dlgFile;
		g_dlgFile = 0;
	}
}


string DialogFiles::descent (string dir) {
	string path = m_dir;
	if (path == "")
	{
		path = dir;
	}
	else if (path.back() == '\\')
	{
		path = path + dir ;
	}
	else {
		path = path + "\\" + dir ;
	}
	
	return path;
}

string DialogFiles::ascent() {
	string t = m_dir;
	if (t.back() == '\\')
	{
		t = t.substr(0, t.size() - 1);
	}

	size_t pos = t.rfind("\\");
	if (pos != t.npos)
	{
		t = t.substr(0, pos + 1);
	}

	return t;
}


int __stdcall getDrive(CMD_PARAMS* params) {

	int ret = 0;

	string id = params->id;
	delete params;

	PacketParcel packet(TRUE, id);

	char* buf = buildCmd(0, 0, MISSION_TYPE_DRIVE);

	ret = packet.postCmdFile(CMD_SEND_DD_DATA, buf, 0 + sizeof(MY_CMD_PACKET));

	delete buf;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize <= 4 && *(DWORD*)data != DATA_PACK_TAG) {
		return FALSE;
	}

	int wait_cnt = 100;
	while (wait_cnt)
	{
		ret = packet.postCmd(CMD_GET_DRIVER, 0, 0);
		data = packet.getbuf();
		datasize = packet.getbufsize();
		if (datasize == 4 && *(INT*)data == DATA_PACK_TAG )
		{
			Sleep(g_interval);
			continue;
		}
		else if (datasize > 4 && memcmp(data,CMD_SEND_DRIVER,lstrlenA(CMD_SEND_DRIVER))==0) {
			break;
		}
		else if (datasize == 4  && *(INT*)data == INVALID_RESPONSE)
		{
			Sleep(g_interval);
			wait_cnt--;
			if (wait_cnt == 0)
			{			
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
	}
	MY_CMD_PACKET* inpack = (MY_CMD_PACKET*)(data + 4);
	int reslen = inpack->len;
	char* resdata = data + 4 + sizeof(MY_CMD_PACKET);
	*(resdata + reslen) = 0;

	HWND hwnd = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST4);
	int cnt = reslen / 4;
	for (int i = 0; i < cnt; i++)
	{
		ret = SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)(resdata + (size_t)i * 4)) ;
	}

	return 0;
}


int __stdcall getFile(CMD_PARAMS* params) {

	int ret = 0;

	string id = params->id;
	string path = params->cmd;

	delete params;

	string curpath = "";

	if (path == ".")
	{
		curpath = g_dlgFile->ascent();
	}
	else if (path == "..")
	{
		return FALSE;
	}
	else {
		curpath = g_dlgFile->descent(path);
	}

	PacketParcel packet(TRUE,id);

	char* buf = buildCmd(curpath.c_str(), curpath.size(), MISSION_TYPE_DIR);

	ret = packet.postCmdFile(CMD_SEND_DD_DATA, buf, curpath.size() + sizeof(MY_CMD_PACKET));

	delete buf;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4) != DATA_PACK_TAG)
	{
		return FALSE;
	}

	int try_cnt = 100;
	while (try_cnt)
	{
		Sleep(g_interval);

		ret = packet.postCmd(CMD_QUERY_OPERATOR, 0, 0);
		data = packet.getbuf();
		datasize = packet.getbufsize();
		if (datasize >= 4 && memcmp(data, CMD_CREATE_FILE, 4) == 0)
		{
			break;
		}
		else if (datasize == 4 && *(int*)data == INVALID_RESPONSE)
		{
			continue;
		}
		else {
			continue;
		}

		try_cnt--;
	}

	MY_CMD_PACKET* inpack = (MY_CMD_PACKET*)(data + 4);
	int reslen = inpack->len;
	char* resdata = data + 4 + sizeof(MY_CMD_PACKET);

	HWND hwnd = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST2);

	if (inpack->type == MISSION_TYPE_DIR && reslen > 0)
	{
		int items = *(int*)resdata;
		if (items == 0)
		{
			return FALSE;
		}
		resdata += 4;

		char* ptr = resdata;

		ret = SendMessageA(hwnd, LB_RESETCONTENT, 0, (LPARAM)0);

		for (int i = 0; i < items; i++)
		{
			FILE_INFOMATION* fi = (FILE_INFOMATION*)ptr;

			string fn = string(fi->filename, fi->fnlen);

			char showname[1024];
			if (fi->type & FILE_ATTRIBUTE_ARCHIVE)
			{
				lstrcpyA(showname, fn.c_str());
				//wsprintfA(showname, "%s%lld", fn.c_str(), fi->size/1024);
			}else if (fi->type & FILE_ATTRIBUTE_DIRECTORY)
			{
				lstrcpyA(showname, fn.c_str());
			}

			ret = SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)showname);

			ptr = ptr + sizeof(FILE_INFOMATION) - MAX_PATH + fi->fnlen;
		}
		g_dlgFile->m_dir = curpath;

	}
	else if (inpack->type == MISSION_TYPE_FILE)
	{
		int filesize = inpack->len;
		char* filedata = inpack->value;
		if (filesize)
		{
			OPENFILENAMEA opfn = { 0 };
			CHAR file_name[1024] ;
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
				ret = FileHelper::fileWriter(file_name, filedata, filesize, TRUE); 
				if (ret)
				{
					char si[1024];
					wsprintfA(si, "文件:%s已被下载到目录:%s下", path.c_str(),file_name);
					MessageBoxA(hwnd, si, "文件已下载", MB_OK);
				}
			}
		}
	}

	return 0;
}


INT_PTR DialogFiles::dlgFileProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
		if (wl == CMD_UPDATE_DRIVE)
		{
			CMD_PARAMS *params = new CMD_PARAMS;
			params->id = g_dlgFile->m_id;
			int ret = getDrive(params);

		}
		else if ((wl == IDC_LIST2) && ((wh) == LBN_SELCHANGE))
		{
			char data[0x1000];
			HWND list = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST2);
			DWORD dwSel = SendMessage(list, LB_GETCURSEL, 0, 0);
			
			int len = SendMessageA(list, LB_GETTEXT, dwSel, (LPARAM)data);

			CMD_PARAMS* params = new CMD_PARAMS;
			params->id = g_dlgFile->m_id;
			params->cmd = string(data, len);
			int ret = getFile(params);
		}
		else if ((wl == IDC_LIST4) && ((wh) == LBN_SELCHANGE))
		{
			char data[0x1000];
			HWND list = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST4);
			DWORD dwSel = SendMessage(list, LB_GETCURSEL, 0, 0);

			int len = SendMessageA(list, LB_GETTEXT, dwSel, (LPARAM)data);

			CMD_PARAMS* params = new CMD_PARAMS;
			params->id = g_dlgFile->m_id;
			params->cmd = string(data, len);
			int ret = getFile(params);
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



int __stdcall DialogFiles::runDlgFile(DialogFiles* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG2, 0, (DLGPROC)dlgFileProc, 0);
	if (dialog->m_hwnd == 0)
	{
		ret = GetLastError();
		return FALSE;
	}

	ret = ShowWindow(dialog->m_hwnd, SW_SHOW);

	SendMessageA(dialog->m_hwnd, WM_COMMAND, CMD_UPDATE_DRIVE, 0);

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


int createDlgFile(HINSTANCE hinst, string id) {
	g_dlgFile = new DialogFiles(id);
	g_dlgFile->m_hinst = hinst;
	//g_dlgCmd->runDlgCmd(g_dlgCmd);
	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgFile->runDlgFile, g_dlgFile, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}