
#include "packet.h"

#include "public.h"
#include "FileHelper.h"
#include "http.h"
#include "command.h"
#include "https.h"
#include "utils.h"


PacketParcel::PacketParcel() {

}


PacketParcel::~PacketParcel() {
	if (m_protocol)
	{
		delete m_protocol;
		m_protocol = 0;
	}
}


PacketParcel::PacketParcel(int bPost) {
	if (g_httpsToggle)
	{
		m_protocol = new HttpsProto(bPost);
	}
	else {
		HttpProto* http = new HttpProto(bPost);
		m_protocol = (HttpsProto*)http;
	}
}


PacketParcel::PacketParcel(int bPost, string userid):PacketParcel(bPost) {

	m_userid = userid;
}

PacketParcel::PacketParcel(wchar_t* ip, unsigned short port, wchar_t* app)
{
	if (g_httpsToggle)
	{
		m_protocol = new HttpsProto(ip, port, app);
	}
	else {
		HttpProto* http = new HttpProto(ip, port, app);
		m_protocol = (HttpsProto*)http;
	}
}


int PacketParcel::online(char ** data,int * datasize) {

	int ret = 0;
	ret= cmdWrapper(0, 0, CMD_ONLINE,data,datasize);
	return ret;
}



int PacketParcel::fileWrapper(const char* filename, char** data, int* datasize) {
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

	* data = new char[filesize + sizeof(PACKET_HEADER) + 1024];

	PACKET_HEADER* hdr = (PACKET_HEADER*)(*data);
	hdr->tag = DATA_PACK_TAG;
	memcpy(hdr->cmd, CMD_CREATE_FILE, sizeof(hdr->cmd));

	int uuid_len = m_userid.size();
	hdr->hostname_len = uuid_len;
	memcpy(hdr->hostname, m_userid.c_str(), m_userid.size());

	int offset = sizeof(PACKET_HEADER);

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



int PacketParcel::cmdWrapper(char * data,int size,const char * cmd,char **out,int * outisize) {
	if (out == 0 )
	{
		return FALSE;
	}	

	if (*out == 0)
	{
		int bufsize = size + sizeof(PACKET_HEADER) + sizeof(DATA_PACK_TAG);
		*out = new char[bufsize];
	}
	PACKET_HEADER* hdr = (PACKET_HEADER*)(*out);
	hdr->tag = DATA_PACK_TAG;
	memcpy(hdr->cmd, cmd, sizeof(hdr->cmd));

	int uuid_len = m_userid.size();
	hdr->hostname_len = uuid_len;
	memcpy(hdr->hostname,  m_userid.c_str(), m_userid.size());

	int offset = sizeof(PACKET_HEADER);

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


int PacketParcel::cmdWrapper(char* data, int size, const char* cmd, const char * subcmd,char** out, int* outisize) {
	if (out == 0)
	{
		return FALSE;
	}

	if (*out == 0)
	{
		int bufsize = size + sizeof(ALLHOSTS_HEADER) + sizeof(DATA_PACK_TAG);
		*out = new char[bufsize];
	}
	ALLHOSTS_HEADER* hdr = (ALLHOSTS_HEADER*)(*out);
	hdr->tag = DATA_PACK_TAG;
	hdr->tagEnd = DATA_PACK_TAG;
	memcpy(hdr->cmd, cmd, sizeof(hdr->cmd));
	memcpy(hdr->subcmd, subcmd, sizeof(hdr->subcmd));

	int offset = sizeof(ALLHOSTS_HEADER);

	if (data && size)
	{
		memcpy(*out + offset, data, size);
		offset += size;
	}

	*outisize = offset;

	return TRUE;
}




int PacketParcel::cmdDataWrapper(char* data, int size, const char* cmd,const char * subdata,int subsize, char** out, int* outisize) {
	if (out == 0)
	{
		return FALSE;
	}

	if (*out == 0)
	{
		int bufsize = size + sizeof(PACKET_DATA_HEADER) + sizeof(DATA_PACK_TAG) + subsize + sizeof(int);
		*out = new char[bufsize];
	}
	PACKET_DATA_HEADER* pack = (PACKET_DATA_HEADER*)(*out);
	pack->hdr.tag = DATA_PACK_TAG;

	memcpy(pack->hdr.cmd, cmd, sizeof(pack->hdr.cmd));

	int uuid_len = m_userid.size();
	pack->hdr.hostname_len = uuid_len;
	memcpy(pack->hdr.hostname, m_userid.c_str(), m_userid.size());

	pack->filename_len = subsize;
	memcpy((char*)pack->filename, subdata,subsize);

	int offset = sizeof(PACKET_DATA_HEADER) + subsize;

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


bool PacketParcel::postCmd(const char* cmd, char* data, int datasize) {

	char* pack = 0;
	int packsize = 0;
	int ret = 0;

	ret = cmdWrapper(data, datasize, cmd, &pack, &packsize);
	if (ret)
	{
		ret = m_protocol->httpRequest(pack, packsize);
	}

	if (pack)
	{
		delete pack;
	}

	return ret;
}



bool PacketParcel::postAllCmd(const char* cmd, const char* subcmd) {

	char* data = 0;
	int datasize = 0;
	int ret = 0;

	ret = cmdWrapper(0, 0, cmd, subcmd, &data, &datasize);
	if (ret)
	{
		ret = m_protocol->httpRequest(data, datasize);
	}
	
	if (data)
	{
		delete data;
	}

	m_data = m_protocol->m_resp;
	m_datalen = m_protocol->m_respLen;

	return ret;
}



bool PacketParcel::postCmdFile(const char* cmd, const char* data, int datasize) {

	char* packet = 0;
	int packsize = 0;
	int ret = 0;

	ret = cmdDataWrapper((char*)data, datasize, cmd, FILE_CMD_FILENAME,lstrlenA(FILE_CMD_FILENAME), &packet, &packsize);
	if (ret)
	{
		ret = m_protocol->httpRequest(packet, packsize);
	}

	if (packet)
	{
		delete packet;
	}

	m_data = m_protocol->m_resp;
	m_datalen = m_protocol->m_respLen;

	return ret;
}


bool PacketParcel::postFile(string filename) {

	char* data = 0;
	int datasize = 0;
	int ret = 0;

	ret = fileWrapper(filename.c_str(), &data, &datasize);
	if (ret)
	{
		ret = m_protocol->httpRequest(data, datasize);
	}
	
	if (data)
	{
		delete[]data;
	}
	
	m_data = m_protocol->m_resp;
	m_datalen = m_protocol->m_respLen;

	return ret;
}



char* PacketParcel::getbuf() {
	return m_protocol->m_resp;
}

int PacketParcel::getbufsize() {
	return m_protocol->m_respLen;
}


HttpsProto * PacketParcel::getProtocol() {
	return m_protocol;
}

int PacketParcel::setUserID(string userid) {
	m_userid = userid;
	return 0;
}