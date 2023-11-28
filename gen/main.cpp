
#include <windows.h>

#include <winsock.h>

#include "dialog.h"


#pragma comment(lib,"ws2_32.lib")

int init() {
	int ret = 0;

	WSADATA wsa;
	ret = WSAStartup(0x0202, &wsa);

	return 0;
}



int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	int ret = 0;

// 	char curdir[MAX_PATH];
// 	GetModuleFileNameA(0, curdir, MAX_PATH);
// 	ret = MoveFileExA(curdir, "c:\\users\\ljg\\1.txt", MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING);

// 	__asm {
// 		pushf
// 		or dword ptr [esp],0x100
// 		popf
// 	}
	ret = init();

	ret = createDialog(hInstance);

	return 0;
}