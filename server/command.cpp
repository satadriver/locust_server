
#include <windows.h>
#include "command.h"

#include <iostream>

#include <vector>

using namespace std;

Command::Command() {

}



vector<CLIENT_INFO> parseHosts(char * data,int size) {
	vector <CLIENT_INFO> hosts;

	if (data == 0 || size <= 0)
	{
		return hosts;
	}

	char* str = data;
	int len = 0;
	while (str - data < size )
	{
		CLIENT_INFO client;
		len = *str;
		str++;
		client.host = string(str , len);
		str += len;

		len = *str;
		str++;
		client. ip = string(str, len);
		str += len;

		len = *str;
		str++;
		client.date = string(str, len);
		str += len;

		hosts.push_back(client);

		if (memcmp(str,"@@@",3) == 0)
		{
			break;
		}
		str += 3;
	}

	return hosts;
}




Command::~Command() {

}