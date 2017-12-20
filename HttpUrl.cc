#include "HttpUrl.h"
#include "CommonType.h"
#include "string"
#include "stdio.h"
#include "stdlib.h"
using namespace std;
int HttpUrl::Parse()
{
	int offset = 0;
	string strUrl = m_pUrl;
	int iHostEndPos=0;
	int isConnect = FALSE;
	if(strUrl[0] != '/')
	{
			if(strUrl.substr(0,8) == string("https://"))
			{
					m_iScheme = PROTOCOL_HTTPS;
					offset = 8;
			}
			else
			{
					if(strUrl.substr(0,7) == string("http://"))
					{
							m_iScheme = PROTOCOL_HTTP;
							offset = 7;
					}
					else
					{
							m_iScheme = PROTOCOL_HTTP;
							isConnect = TRUE;
					}
			}
			if(!isConnect)
				iHostEndPos = strUrl.find("/",offset);
			else
				iHostEndPos = strUrl.length();
			string strHostFull = strUrl.substr(offset,iHostEndPos-offset);
			int port = 80;
			string strHost;
			int portpos = strHostFull.find(":");
			if(portpos != string::npos)
			{
					strHost = strHostFull.substr(0,portpos);
					string strPort = strHostFull.substr(portpos+1);
					port = atoi(strPort.c_str()); 
			}
			else
			{
					strHost = strHostFull;
					port = 80;
			}
			m_iPort = port;
			m_pHost = strHost;
	}
	if(iHostEndPos != string::npos)
		m_pRequestString = strUrl.substr(iHostEndPos);
	else
		m_pRequestString = "/";
	return TRUE;
}


HttpUrl::HttpUrl(char* pUrl)
{
	m_pUrl = string(pUrl);
	m_iPort = 80;
}

HttpUrl::HttpUrl()
{
}

char* HttpUrl::GetHost()
{
	return (char*)m_pHost.c_str();
}
int HttpUrl::GetPort()
{
	return m_iPort;
}
char* HttpUrl::ToString()
{
	return (char*)m_pRequestString.c_str();
}
char* HttpUrl::ToFullString()
{
	return (char*)m_pUrl.c_str();
}
