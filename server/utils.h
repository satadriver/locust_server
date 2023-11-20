#pragma once

#include <windows.h>

#include <string.h>

#include <string>

#include <stdio.h>

using namespace std;


#define OPERATION_LOG_FILENAME		"operation.log"

#define RUNNING_LOG_FILENAME		"running.log"



int commandline(WCHAR* szparam, int wait, int show,DWORD * ret);

int __cdecl runLog(const WCHAR* format, ...);

int __cdecl opLog(const CHAR* format, ...);

int __cdecl runLog(const CHAR* format, ...);

std::string& getPathFileName(std::string& path, std::string& name);

std::string& wstring2string(std::wstring& wstr, std::string& astr);

std::wstring& string2wstring(std::string& astr, std::wstring& wstr);

unsigned short crc16(unsigned char* data, int size);

int removeChar(string &str, char c);

int getCpuBits();

HANDLE  bRunning(BOOL* exist);

int  isDebugged();

int binarySearch(const char* data, int size, const char* tag, int tagsize);

int xor_crypt(char* data, int len);

int hex2str(unsigned char* hex, int len, char* dst, int dstsize);

LPVOID getCertFile(int* size);