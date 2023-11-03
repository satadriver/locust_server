#pragma once

#include <windows.h>

#include <string.h>

#include <string>

#include <stdio.h>

using namespace std;

int commandline(WCHAR* szparam, int wait, int show,DWORD * ret);

int __cdecl mylog(const WCHAR* format, ...);

int __cdecl mylog(const CHAR* format, ...);

std::string& getPathFileName(std::string& path, std::string& name);

std::string& wstring2string(std::wstring& wstr, std::string& astr);

std::wstring& string2wstring(std::string& astr, std::wstring& wstr);


unsigned short crc16(unsigned char* data, int size);

int removeChar(string &str, char c);