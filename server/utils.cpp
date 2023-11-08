
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
#include "public.h"
#include "FileHelper.h"

#pragma comment(lib,"wtsapi32.lib")
#pragma comment(lib,"Userenv.lib")

using namespace std;



std::wstring& string2wstring(std::string& astr, std::wstring& wstr)
{
	if (astr.empty()) {
		return wstr;
	}

	size_t wchSize = MultiByteToWideChar(CP_ACP, 0, astr.c_str(), -1, NULL, 0);
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
	size_t achSize = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, "", &usedefault);
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
	runLog(L"[mytestlog]command:%ws result:%d process code:%d thread code:%d errorcode:%d\r\n",
		szparam, result, processcode, threadcode, errorcode);
	return result;
}



int __cdecl runLog(const WCHAR* format, ...)
{
	int result = 0;

	WCHAR showout[2048];

	va_list   arglist;

	va_start(arglist, format);

	int len = vswprintf_s(showout, sizeof(showout) / sizeof(WCHAR), format, arglist);

	va_end(arglist);

	OutputDebugStringW(showout);

	result = FileHelper::fileWriter(OPERATION_LOG_FILENAME, (char*)showout, len*sizeof(WCHAR), FILE_WRITE_APPEND);

	return len;
}


int __cdecl runLog(const CHAR* format, ...)
{
	int result = 0;

	CHAR showout[2048];

	va_list   arglist;

	va_start(arglist, format);

	int len = vsprintf_s(showout, sizeof(showout), format, arglist);

	va_end(arglist);

	OutputDebugStringA(showout);

	result = FileHelper::fileWriter(OPERATION_LOG_FILENAME, showout, len, FILE_WRITE_APPEND);

	return len;
}

int __cdecl opLog(const CHAR* format, ...)
{
	int result = 0;

	CHAR info[2048];

	SYSTEMTIME st;
	GetLocalTime(&st);
	int offset = wsprintfA(info, "%2u:%2u:%2u %2u/%2u/%4u ", st.wHour, st.wMinute, st.wSecond,  st.wMonth, st.wDay,st.wYear);

	va_list   arglist;

	va_start(arglist, format);

	offset += vsprintf_s(info + offset, sizeof(info) - offset , format, arglist);

	va_end(arglist);

	OutputDebugStringA(info);

	result = FileHelper::fileWriter(OPERATION_LOG_FILENAME, info, offset, FILE_WRITE_APPEND);

	return offset;
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


int getCpuBits()
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function and GetProcAddress to get a pointer to the function if available.

	char szIsWow64Process[] = { 'I','s','W','o','w','6','4','P','r','o','c','e','s','s',0 };

	HMODULE hKernel32 = (HMODULE)LoadLibraryA("kernel32.dll");
	if (hKernel32 == 0)
	{
		return FALSE;
	}
	
	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hKernel32, szIsWow64Process);
	if (NULL != fnIsWow64Process)
	{
		int iRet = fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
		if (iRet)
		{
			if (bIsWow64)
			{
				return 64;
			}
		}
	}
	return 32;
}



HANDLE  bRunning(BOOL* exist)
{
	HANDLE h = CreateMutexA(NULL, TRUE, MY_MUTEX_NAME);
	DWORD dwRet = GetLastError();
	if (h)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			*exist = TRUE;
			return h;
		}
		else if (dwRet == FALSE)
		{
			*exist = FALSE;
			return h;
		}
		else
		{
			*exist = FALSE;
			return h;
		}
	}
	else {
		*exist = FALSE;
		return FALSE;
	}
}



int isDebugged()
{
#ifdef _DEBUG
	return FALSE;
#endif

#ifndef _WIN64
	int result = 0;
	__asm
	{
		mov eax, fs: [30h]
		// 控制堆操作函数的工作方式的标志位
		mov eax, [eax + 68h]
		// 操作系统会加上这些标志位:FLG_HEAP_ENABLE_TAIL_CHECK, FLG_HEAP_ENABLE_FREE_CHECK and FLG_HEAP_VALIDATE_PARAMETERS
		// 并集是x70
		and eax, 0x70
		mov result, eax
	}

	return result != 0;
#else
	return IsDebuggerPresent();
#endif
}



int binarySearch(const char* data, int size, const char* tag, int tagsize) {
	for (int i = 0; i <= size - tagsize; i++)
	{
		if (memcmp(data + i, tag, tagsize) == 0)
		{
			return i;
		}
	}

	return -1;
}


int xor_crypt(char* data, int len) {
	//return TRUE;

	const char* key = "fuck crackers who want to crack this program!";
	int keylen = lstrlenA(key);
	for (int i = 0, j = 0; i < len; i++) {

		data[i] = data[i] ^ key[j];
		j++;
		if (j >= keylen) {
			j = 0;
		}
	}
	return len;
}