
#include "packet.h"

#include "public.h"
#include "FileHelper.h"


PacketHeader::PacketHeader() {

}

PacketHeader::~PacketHeader() {

}


int PacketHeader::online(char ** data,int * datasize) {

	int ret = 0;
	int size = 0;
	ret= cmdWrapper(0, 0, CMD_ONLINE,data,datasize);
	return ret;
}



int PacketHeader::fileWrapper(const char* filename, char** data, int* datasize) {
	int ret = 0;

	HANDLE hf = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD sizehigh = 0;
	int filesize = GetFileSize(hf, &sizehigh);
	if (sizehigh || filesize == 0)
	{
		CloseHandle(hf);
		return FALSE;
	}

	* data = new char[filesize + sizeof(DATA_PADDING) + 1024];

	DATA_PADDING* hdr = (DATA_PADDING*)(*data);
	hdr->tag = DATA_PACK_TAG;
	memcpy(hdr->cmd, CMD_CREATE_FILE, sizeof(hdr->cmd));

	int uuid_len = g_uuid_len;
	hdr->hostname_len = uuid_len;
	memcpy(hdr->hostname, g_uuid, uuid_len);

	int offset = sizeof(DATA_PADDING);

	DWORD cnt = 0;
	ret = ReadFile(hf, *data + offset, filesize, &cnt, 0);
	if (ret == 0)
	{
	}
	CloseHandle(hf);
	offset += filesize;

	*(DWORD*)(*data + offset) = DATA_PACK_TAG;
	offset += sizeof(DATA_PACK_TAG);

	*datasize = offset;

	return offset;
}



int PacketHeader::cmdWrapper(char * data,int size,const char * cmd,char **out,int * outisize) {
	if (out == 0 )
	{
		return FALSE;
	}	

	if (*out == 0)
	{
		int bufsize = size + sizeof(DATA_PADDING) + sizeof(DATA_PACK_TAG);
		*out = new char[bufsize];
	}
	DATA_PADDING* hdr = (DATA_PADDING*)(*out);
	hdr->tag = DATA_PACK_TAG;
	memcpy(hdr->cmd, cmd, sizeof(hdr->cmd));

	int uuid_len = g_uuid_len;
	hdr->hostname_len = uuid_len;
	memcpy(hdr->hostname, g_uuid, uuid_len);

	int offset = sizeof(DATA_PADDING);

	if (data && size)
	{
		memcpy(*out + offset, data, size);
		offset += size;
	}

	*(DWORD*)(*out + offset) = DATA_PACK_TAG;
	offset += sizeof(DATA_PACK_TAG);

	*outisize = offset;
	
	return TRUE;
}


int PacketHeader::cmdWrapper(char* data, int size, const char* cmd, const char * subcmd,char** out, int* outisize) {
	if (out == 0)
	{
		return FALSE;
	}

	if (*out == 0)
	{
		int bufsize = size + sizeof(SUBCMD_HEADER) + sizeof(DATA_PACK_TAG);
		*out = new char[bufsize];
	}
	SUBCMD_HEADER* hdr = (SUBCMD_HEADER*)(*out);
	hdr->tag = DATA_PACK_TAG;
	hdr->tagEnd = DATA_PACK_TAG;
	memcpy(hdr->cmd, cmd, sizeof(hdr->cmd));
	memcpy(hdr->subcmd, subcmd, sizeof(hdr->subcmd));

	int offset = sizeof(SUBCMD_HEADER);

	if (data && size)
	{
		memcpy(*out + offset, data, size);
		offset += size;
	}

	*outisize = offset;

	return TRUE;
}




int PacketHeader::cmdStrWrapper(char* data, int size, const char* cmd,const char * str, char** out, int* outisize) {
	if (out == 0)
	{
		return FALSE;
	}

	if (*out == 0)
	{
		int bufsize = size + sizeof(PACK_STR_HEADER) + sizeof(DATA_PACK_TAG)+1024;
		*out = new char[bufsize];
	}
	PACK_STR_HEADER* pack = (PACK_STR_HEADER*)(*out);
	pack->hdr.tag = DATA_PACK_TAG;

	memcpy(pack->hdr.cmd, cmd, sizeof(pack->hdr.cmd));

	int uuid_len = g_uuid_len;
	pack->hdr.hostname_len = uuid_len;
	memcpy(pack->hdr.hostname, g_uuid, uuid_len);

	int fnlen = lstrlenA(str);

	pack->filename_len = fnlen;
	lstrcpyA((char*)pack->filename, str);

	int offset = sizeof(PACK_STR_HEADER) + fnlen;

	if (data && size)
	{
		memcpy(*out + offset, data, size);
		offset += size;
	}

	*(DWORD*)(*out + offset) = DATA_PACK_TAG;
	offset += sizeof(DATA_PACK_TAG);

	*outisize = offset;

	return TRUE;
}