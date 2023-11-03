#pragma once

#include <vector>
#include <iostream>


using namespace std;

#define FILE_CMD_FILENAME "TransmitFile.dat"

#pragma pack(1)

typedef struct  
{
	string host;
	string ip;
	string date;

}CLIENT_INFO;

#pragma pack()

class Command {
	Command();
	~Command();
};


vector<CLIENT_INFO> parseHosts(char* data, int size);