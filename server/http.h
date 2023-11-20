#pragma once


/*
    http ContentType References:
    https://www.w3.org/Protocols/rfc1341/7_2_Multipart.html
*/

#include <vector>
#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>

#pragma comment(lib,"winhttp.lib")

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string  tstring;
#endif

using namespace std;





#define BUFFER_SIZE         0X4000

#define MAX_SIZE            0x1000

#define MY_PHP_SERVER       L"/index.php"

#define HTTPS_PORT          443

#define HTTP_PORT           80

#define CERT_PROTECT_KEY    "1234"



class HttpProto
{
public:
    HttpProto(int action);

    HttpProto();

    HttpProto(wchar_t* ip, unsigned short port, wchar_t* app);

    virtual ~HttpProto();

    virtual bool postTest(string myfile,char * data,int filesize);

    bool getCmd(const char* cmd);

    bool getSubCmd(const char* cmd, const char* subcmd);

    bool getCmdStr(const char* cmd, const char* str);



    bool setUrl(wstring url);

	bool setPort(USHORT port);

	bool setIP(USHORT port);

    unsigned long BuildPostEntity(char* filebuf, unsigned long filesize, const char* filename, char** entity);

    tstring& BuildBoundary(tstring& bdrout);
    std::string& BuildFileParamString(std::string& key, std::string& value, std::string& mstr);
    std::string& BuildOtherParamString(std::string& key, std::string& value, std::string& mstr);
    std::string& BuildEndBoundaryString(std::string& endflag);

    virtual bool httpRequest(char* data, int datasize);

    std::wstring& BuildGzip(std::wstring& lengthstr);

    std::wstring& BuildContentLength(unsigned long length, std::wstring& lengthstr);

    std::wstring& BuildHeaderContentType(std::wstring& strout);

    wstring m_action;
    tstring m_boundary;
    wchar_t m_ip[MAX_PATH];
    wchar_t m_app[MAX_PATH];
    unsigned short m_port;
    char* m_inCmd;

	int m_respLen;
	char* m_resp;

protected:
};

int uploadFileTest(const char* filename);

unsigned long makeFileEntity(char* filebuf, unsigned long filesize, const char* filename, char** entity);

