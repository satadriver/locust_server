
#include "utils.h"
#include <windows.h>
#include <Shlobj.h>
#include <stdio.h>
#include <io.h>
#include <UserEnv.h>
#include <tlhelp32.h>
#include <WtsApi32.h>
#include "utils.h"
#include <Shlwapi.h>
#include <Psapi.h>

#include <iostream>
#include <string>

#pragma comment(lib,"wtsapi32.lib")
#pragma comment(lib,"Userenv.lib")

using namespace std;



std::wstring& string2wstring(std::string& astr, std::wstring& wstr)
{
	if (astr.empty()) {
		return wstr;
	}

	DWORD wchSize = MultiByteToWideChar(CP_ACP, 0, astr.c_str(), -1, NULL, 0);
	wchar_t* pwchar = new wchar_t[wchSize+16];
	ZeroMemory(pwchar, wchSize * sizeof(wchar_t)+16);
	MultiByteToWideChar(CP_ACP, 0, astr.c_str(), -1, pwchar, wchSize+16);
	wstr = pwchar;
	delete[]pwchar;
	pwchar = NULL;
	return wstr;
}

std::string& wstring2string(std::wstring& wstr, std::string& astr)
{
	if (wstr.empty()) {
		return astr;
	}
	BOOL usedefault = TRUE;
	DWORD achSize = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, "", &usedefault);
	char* pachar = new char[achSize + 16];
	ZeroMemory(pachar, achSize * sizeof(char) + 16);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pachar, achSize + 16, "", &usedefault);
	astr = pachar;
	delete []pachar;
	pachar = NULL;
	return astr;
}


std::string& getPathFileName(std::string& path, std::string& name)
{
	if (path.empty()) {
		return name;
	}

	std::string::size_type pos = path.rfind('\\');
	if (pos == std::string::npos) {
		name = path;
		return path;
	}

	name = path.substr(pos + 1);
	return name;
}



int commandline(WCHAR* szparam, int wait, int show,DWORD * ret) {
	int result = 0;

	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD processcode = 0;
	DWORD threadcode = 0;

	si.cb = sizeof(STARTUPINFOW);
	si.lpDesktop = (WCHAR*)L"WinSta0\\Default";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = show;
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;

	result = CreateProcessW(0, szparam, 0, 0, 0, 0, 0, 0, &si, &pi);
	int errorcode = GetLastError();
	if (result) {
		if (wait)
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeThread(pi.hProcess, &threadcode);
			GetExitCodeProcess(pi.hProcess, &processcode);
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	mylog(L"[mytestlog]command:%ws result:%d process code:%d thread code:%d errorcode:%d\r\n", szparam, result, processcode, threadcode, errorcode);
	return result;
}



int __cdecl mylog(const WCHAR* format, ...)
{
	int result = 0;

	WCHAR procname[MAX_PATH];
	result = GetModuleFileNameW(0, procname, MAX_PATH);
	WCHAR* process_name = wcsrchr(procname, L'\\');
	if (process_name)
	{

	}
	else {
	}

	WCHAR showout[2048];

	va_list   arglist;

	va_start(arglist, format);

	int len = vswprintf_s(showout, sizeof(showout) / sizeof(WCHAR), format, arglist);

	va_end(arglist);

	OutputDebugStringW(showout);

	return len;
}




int __cdecl mylog(const CHAR* format, ...)
{
	int result = 0;

	CHAR procname[MAX_PATH];
	result = GetModuleFileNameA(0, procname, MAX_PATH);
	CHAR* process_name = strchr(procname, L'\\');
	if (process_name)
	{

	}
	else {
	}

	CHAR showout[2048];

	va_list   arglist;

	va_start(arglist, format);

	int len = vsprintf_s(showout, sizeof(showout) , format, arglist);

	va_end(arglist);

	OutputDebugStringA(showout);

	return len;
}



unsigned short crc16(unsigned char* data, int size) {

	int cnt = size >> 1;
	int mod = size % 2;

	unsigned int v = 0;

	unsigned short* crcdata = (unsigned short*)data;

	for (int i = 0; i < cnt; i++)
	{
		v += crcdata[i];
	}

	if (mod)
	{
		v += data[cnt * 2];
	}

	unsigned int high16 = v >> 16;
	v = v & 0xffff;
	v += high16;
	return v;
}



int removeChar(string &str, char c) {
	char cstr[2] = { 0 };
	cstr[0] = c;
	int pos = -1;
	do 
	{
		pos = str.find(cstr);
		if (pos != -1)
		{
			str = str.replace(pos, 1, "");
		}
		
	} while (pos != -1);
	
	return TRUE;
}