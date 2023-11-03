#pragma once

#include <windows.h>

#include <iostream>
#include "http.h"

using namespace std;

class HttpsProto:public HttpProto
{
public:
	HttpsProto();
	HttpsProto(int action);

	HttpsProto(wchar_t* ip, unsigned short port, wchar_t* app);

	virtual ~HttpsProto();

	//virtual bool postTest(string myfile, char* data, int filesize);	

	virtual bool postFileTest(string filename, char* backdata,int * backsize);

	virtual bool postCmdTest(const char* cmd, char* backdata, int* backsize);


	virtual bool httpRequest(char* data, int datasize);

protected:
};

int uploadHttpsFile(const char* filename);