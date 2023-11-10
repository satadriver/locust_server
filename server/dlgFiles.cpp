

#include "dlgfiles.h"

#include <stdio.h>
#include <iostream>
#include "resource.h"
#include "dlgCmd.h"
#include "dialog.h"
#include "public.h"
#include "mission.h"
#include "packet.h"
#include "FileHelper.h"
#include "dlgrename.h"

#include <map>
#include "utils.h"

using namespace std;


DialogFiles* g_dlgFile = 0;

map<string , vector<string> > g_mapDir;


DialogFiles::DialogFiles() {

}

DialogFiles::DialogFiles(string id) {
	m_id = id;
	m_dir = "";
}

DialogFiles::~DialogFiles() {
	if (g_dlgFile)
	{
// 		delete g_dlgFile;
// 		g_dlgFile = 0;
	}
	if (g_mapDir.size())
	{
		g_mapDir.clear();
	}
}

string DialogFiles::setPath(string path) {
	m_dir = path;
	return m_dir;
}

string DialogFiles::getPath() {
	string path = m_dir;
	if (path.back() != '\\')
	{
		path = path + "\\";
	}
	return path;
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

	std::map<string, vector<string>>::iterator it = g_mapDir.find(id);
	if (it == g_mapDir.end())
	{

		PacketParcel packet(TRUE, id);

		char* buf = buildCmd(0, 0, MISSION_TYPE_DRIVE);

		ret = packet.postCmdFile(CMD_BRING_COMMAND, buf, 0 + sizeof(MY_CMD_PACKET));

		delete buf;

		char* data = packet.getbuf();
		int datasize = packet.getbufsize();
		if (datasize <= 4 && *(DWORD*)data != DATA_PACK_TAG) {
			return FALSE;
		}

		int wait_cnt = 30;
		while (wait_cnt)
		{
			ret = packet.postCmd(CMD_TAKE_COMMAND_RESULT, 0, 0);
			data = packet.getbuf();
			datasize = packet.getbufsize();

			if (datasize >= 8 && *(INT*)data == DATA_PACK_TAG) {
				break;
			}
			else if (datasize == 4 && *(INT*)data == INVALID_RESPONSE)
			{
				Sleep(g_interval);
				wait_cnt--;
				if (wait_cnt == 0)
				{
					return FALSE;
				}
			}
			else {
				Sleep(g_interval);
				wait_cnt--;		
				if (wait_cnt == 0)
				{
					return FALSE;
				}
			}
		}

		PACKET_HEADER* hdr = (PACKET_HEADER*)data;
		if (datasize > 8 && memcmp(hdr->hdr.cmd,CMD_PUT_COMMAND_RESULT,lstrlenA(CMD_PUT_COMMAND_RESULT)) == 0)
		{
			MY_CMD_PACKET* inpack = (MY_CMD_PACKET*)(data + sizeof(PACKET_HEADER));
			if (inpack->type == MISSION_TYPE_DRIVE)
			{
				int reslen = inpack->len;
				char* resdata = inpack->value;

				HWND hwnd = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST4);
				int cnt = reslen / 4;
				vector<string > drives;
				for (int i = 0; i < cnt; i++)
				{
					string drive = resdata + (size_t)i * 4;

					ret = SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)(drive.c_str()));

					drives.push_back(drive);
				}

				pair<string, vector<string> > mp = { id,drives };
				g_mapDir.insert(mp);
				return TRUE;
			}
		}
	}
	else {
		HWND hwnd = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST4);
		for (int i = 0;i < it->second.size();i ++)
		{
			string drive = it->second[i];
			ret = SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)(drive.c_str()));	
		}
		return TRUE;
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
		if (path.size() == 3 && path.c_str()[2] == '\\' && path.c_str()[1] == ':')
		{
			curpath = g_dlgFile->setPath(path);
		}
		else {
			curpath = g_dlgFile->descent(path);
		}	
	}

	std::map<string, vector<string>>::iterator it = g_mapDir.find(curpath);
	if (it == g_mapDir.end())
	{
		//continue to process in the next;
	}
	else {
		HWND hwnd = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST2);
		ret = SendMessageA(hwnd, LB_RESETCONTENT, 0, (LPARAM)0);
		for (int i = 0; i < it->second.size(); i++)
		{
			string filename = it->second[i];
			ret = SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)filename.c_str());
		}

		g_dlgFile->setPath(curpath);
		return ret;
	}

	PacketParcel packet(TRUE,id);

	char* bp = buildCmd(curpath.c_str(), curpath.size(), MISSION_TYPE_DIR);

	ret = packet.postCmdFile(CMD_BRING_COMMAND, bp, curpath.size() + sizeof(MY_CMD_PACKET));

	delete bp;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG || *(int*)(data + datasize - 4) != DATA_PACK_TAG)
	{
		return FALSE;
	}

	int try_cnt = 30;
	while (try_cnt)
	{
		Sleep(1000);

		ret = packet.postCmd(CMD_TAKE_COMMAND_RESULT, 0, 0);
		data = packet.getbuf();
		datasize = packet.getbufsize();
		if (datasize > 8 && *(INT*)data == DATA_PACK_TAG)
		{
			break;
		}
		else if (datasize <= 8 && *(int*)data == INVALID_RESPONSE)
		{
			try_cnt--;
			continue;
		}
		else {
			try_cnt--;
			continue;
		}	
	}

	PACKET_HEADER* hdr = (PACKET_HEADER*)(data);
	if (datasize > 8 && memcmp(hdr->hdr.cmd, CMD_PUT_COMMAND_RESULT, lstrlenA(CMD_PUT_COMMAND_RESULT)) == 0)
	{
		MY_CMD_PACKET* inpack = (MY_CMD_PACKET*)(data + sizeof(PACKET_HEADER));
		int reslen = inpack->len;
		char* resdata = inpack->value;

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
			vector<string> fns;
			string filename = "";
			for (int i = 0; i < items; i++)
			{
				FILE_INFOMATION* fi = (FILE_INFOMATION*)ptr;

				filename = string(fi->filename, fi->fnlen);

				if (fi->type & FILE_ATTRIBUTE_ARCHIVE)
				{

				}
				else if (fi->type & FILE_ATTRIBUTE_DIRECTORY)
				{

				}

				ret = SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)filename.c_str());

				ptr = ptr + sizeof(FILE_INFOMATION) - MAX_PATH + fi->fnlen;

				fns.push_back(filename);
			}

			//in case the curpath = id,how to process ?
			pair<string, vector<string> > mp = { curpath,fns };
			g_mapDir.insert(mp);

			g_dlgFile->setPath(curpath);
			//g_dlgFile->m_dir = curpath;
		}
		else if (inpack->type == MISSION_TYPE_FILE)
		{
			opLog("object:%s download file:%s\r\n", id.c_str(), path.c_str());

			int filesize = inpack->len;
			char* filedata = inpack->value;
			if (filesize)
			{
				OPENFILENAMEA opfn = { 0 };
				CHAR file_name[1024];
				opfn.lStructSize = sizeof(OPENFILENAMEA);
				opfn.lpstrFilter = "所有文件\0*.*\0\0";
				//指向一对以空字符结束的过滤字符串的一个缓冲。缓冲中的最后一个字符串必须以两个  NULL字符结束。
				opfn.nFilterIndex = 1;    //指定在文件类型控件中当前选择的过滤器的索引
				opfn.lpstrFile = file_name;
				opfn.lpstrFile[0] = '\0'; //这个缓冲的第一个字符必须是NULL
				opfn.nMaxFile = sizeof(file_name);
				opfn.Flags = 0;			//OFN_FILEMUSTEXIST OFN_PATHMUSTEXIST指定用户仅可以在文件名登录字段中输入已存在的文件的名字。	
				ret = GetOpenFileNameA(&opfn);
				if (ret)
				{
					ret = FileHelper::fileWriter(file_name, filedata, filesize, TRUE);
					if (ret)
					{
						char si[1024];
						wsprintfA(si, "文件:%s已被下载到目录:%s下", path.c_str(), file_name);
						MessageBoxA(hwnd, si, "文件已下载", MB_OK);
					}
				}
			}
		}
	}


	return 0;
}


int __stdcall delFile(CMD_PARAMS* params) {

	int ret = 0;

	string id = params->id;
	string path = params->cmd;

	delete params;

	path = g_dlgFile->getPath() + path;

	PacketParcel packet(TRUE, id);

	char* buf = buildCmd(path.c_str(), path.size(), MISSION_TYPE_DELFILE);

	ret = packet.postCmdFile(CMD_BRING_COMMAND, buf, path.size() + sizeof(MY_CMD_PACKET));

	opLog("object:%s delete file:%s\r\n", id.c_str(), path.c_str());

	delete buf;

	char* data = packet.getbuf();
	int datasize = packet.getbufsize();
	if (datasize < 4 || *(INT*)data != DATA_PACK_TAG )
	{
		return FALSE;
	}

	return TRUE;
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
			if (ret == FALSE)
			{
				ret = getDrive(params);
			}
		}
		else if (wl == IDM_DEL_FILE)
		{
			char data[0x1000];
			HWND list = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST2);
			DWORD dwSel = SendMessage(list, LB_GETCURSEL, 0, 0);

			int len = SendMessageA(list, LB_GETTEXT, dwSel, (LPARAM)data);

			CMD_PARAMS* params = new CMD_PARAMS;
			params->id = g_dlgFile->m_id;
			params->cmd = string(data, len);
			int ret = delFile(params);
		}
		else if (wl == IDM_REN_FILE)
		{
			char data[0x1000];
			HWND list = GetDlgItem(g_dlgFile->m_hwnd, IDC_LIST2);
			DWORD dwSel = SendMessage(list, LB_GETCURSEL, 0, 0);

			int len = SendMessageA(list, LB_GETTEXT, dwSel, (LPARAM)data);

			CMD_PARAMS* params = new CMD_PARAMS;
			params->id = g_dlgFile->m_id;
			params->cmd = string(data, len);

			int ret = createDlgRename(g_dlgFile->m_hinst, g_dlgFile->m_id, g_dlgFile->m_dir, data);
		}
		else if ((wl == IDC_LIST2) && ((wh) == LBN_DBLCLK))
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
		else if ((wl == IDC_LIST4) && ((wh) == LBN_DBLCLK))
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


int DialogFiles::rmenu() {

	HMENU menu = LoadMenuA(0, (LPCSTR)IDR_MENU2);

	HMENU sub = GetSubMenu(menu, 0);
	RECT rect = { 0 };

	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenu(sub, TPM_LEFTBUTTON | TPM_TOPALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hwnd, 0);

	return 0;
}


int fileListBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (msg == WM_RBUTTONDOWN) {

		int ret = g_dlgFile->rmenu();

		return TRUE;
	}
	return CallWindowProcA(g_dlgFile->m_lbProc, hwnd, msg, wparam, lparam);
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

	HWND hwnd = GetDlgItem(dialog->m_hwnd, IDC_LIST2);

	dialog->m_lbProc = (WNDPROC)GetWindowLongPtrA(hwnd, GWLP_WNDPROC);

	ret = SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR)fileListBoxProc);

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

	if (g_dlgFile)
	{
		delete g_dlgFile;
		g_dlgFile = 0;
	}
	return 0;
}


int createDlgFile(HINSTANCE hinst, string id) {
	g_dlgFile = new DialogFiles(id);
	g_dlgFile->m_hinst = hinst;

	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_dlgFile->runDlgFile, g_dlgFile, 0, 0);
	if (h)
	{
		CloseHandle(h);
	}
	return TRUE;
}