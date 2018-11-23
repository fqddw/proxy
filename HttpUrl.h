#ifndef __HTTPURL_H__
#define __HTTPURL_H__
#define PROTOCOL_HTTP 1
#define PROTOCOL_HTTPS 2
#include "string"
using namespace std;
class HttpUrl
{
	public:
		HttpUrl();
		HttpUrl(char*);
		char* GetHost();
		int Parse();
		char* ToString();
		char* GetFullString();
		int GetPort();
	private:
		string m_pUrl;
		string m_pRequestString;
		string m_pHost;
		int m_iPort;
		int m_iScheme;
};
#endif
