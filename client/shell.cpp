

#include "shell.h"

#include "utils.h"



int __stdcall shell(char * cmd) {
	int ret = 0;

	wchar_t wstrcmd[1024];

	ret = mbstowcs(wstrcmd, cmd, sizeof(wstrcmd) / sizeof(wchar_t));

	DWORD result = 0;

	ret = commandline(wstrcmd, TRUE, FALSE, &result);

	return ret;
}



int runShell(char* cmd) {
	HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)shell, cmd, 0, 0);
	if (ht)
	{
		CloseHandle(ht);
	}
	return 0;
}