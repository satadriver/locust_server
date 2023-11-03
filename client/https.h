#pragma once

#include <windows.h>
#include "http.h"

using namespace std;

class HttpsProto:public HttpProto
{
public:
	HttpsProto();
	HttpsProto(int action);

	HttpsProto(wchar_t* ip, unsigned short port, wchar_t* app);

	virtual ~HttpsProto();

	virtual bool postTest(string file, char* data, int filesize);	

	virtual bool postFile(string filename, char* backdata,int * backsize);

	virtual bool postCmd(const char* cmd, char* backdata, int* backsize);

protected:
	virtual bool httpRequest(void* data, unsigned long datasize, char* response, int *len);
};

int uploadHttpsFile(const char* filename);