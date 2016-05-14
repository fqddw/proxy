#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "HttpRequestLine.h"
#include "CommonType.h"
HttpRequestLine::HttpRequestLine():m_pString(0),m_iStringLength(0)
{
}
HttpUrl* HttpRequestLine::GetUrl()
{
	return m_pUrl;
}
char* HttpRequestLine::ToString()
{
	char* pMethod = GetMethodString();
	char* pUrl = m_pUrl->ToString();
	char* pRequestString = new char[strlen(pMethod)+1+strlen(pUrl)+1+8+1];
	pRequestString[strlen(pMethod)+1+strlen(pUrl)+1+8] = '\0';
	memcpy(pRequestString,pMethod,strlen(pMethod));
	memcpy(pRequestString+strlen(pMethod)," ",1);
	memcpy(pRequestString+strlen(pMethod)+1,pUrl,strlen(pUrl));
	memcpy(pRequestString+strlen(pMethod)+1+strlen(pUrl)," ",1);
	memcpy(pRequestString+strlen(pMethod)+1+strlen(pUrl)+1,"HTTP/",5);
	char ver[128] = {'\0'};
	sprintf(ver,"%d.%d",m_iMajorVer,m_iSeniorVer);
	memcpy(pRequestString+strlen(pMethod)+1+strlen(pUrl)+1+5,ver,strlen(ver));
	return pRequestString;
}

char* HttpRequestLine::GetMethodString()
{
	if(m_iMethod == HTTP_METHOD_GET)
		return (char*)"GET";
	if(m_iMethod == HTTP_METHOD_POST)
		return (char*)"POST";
}
int HttpRequestLine::GetMajorVer()
{
	return m_iMajorVer;
}
int HttpRequestLine::GetSeniorVer()
{
	return m_iSeniorVer;
}

int HttpRequestLine::SetUrl(char* pUrl)
{
	m_pUrl = new HttpUrl(pUrl);
	return 0;
}

int HttpRequestLine::SetVersion(int major,int senior)
{
	m_iMajorVer = major;
	m_iSeniorVer = senior;
	return 0;
}

int HttpRequestLine::Parse()
{
	int offset = 0;
	char* pString = m_pString;
	int index=0;
	for(;index<m_iStringLength;index++){
		if(*(pString+index) == ' '){
			break;
		}
	}

	int len = index;
	char* pMethod = new char[len+1];
	memset(pMethod,'\0',len+1);
	memcpy(pMethod,pString,len);
	m_iMethod = GetMethodId(pMethod);
	for(;index<m_iStringLength;index++){
		if(*(pString+index) != ' '){
			break;
		}
	}
	int urlstart = index;
	for(;index<m_iStringLength;index++){
		if(*(pString+index) == ' '){
			break;
		}
	}
	len = index-urlstart;

	char* pUrl = new char[len+1];
	memset(pUrl,'\0',len+1);
	memcpy(pUrl,pString+urlstart,len);
	m_pUrl = new HttpUrl(pUrl);
	for(;index<m_iStringLength;index++){
		if(*(pString+index) != ' '){
			break;
		}
	}

	if(*(pString+index++) == 'H' && *(pString+index++) == 'T' && *(pString+index++) == 'T' && *(pString+index++) == 'P' && *(pString+index++) == '/')
	{	
		char mVer[2] = {'\0'};
		mVer[0] = *(pString+index);
		m_iMajorVer = atoi(mVer);
		index++;
		if(*(pString+index)!='.')
			return FALSE;
		index++;
		char sVer[2]= {'\0'};
		sVer[0] = *(pString+index);
		m_iSeniorVer = atoi(sVer);
	}
	return 0;
}

int HttpRequestLine::GetMethodId(char* pMethod)
{
	if(strlen(pMethod) == 3)
	{
		if(pMethod[0] == 'G' && pMethod[1] == 'E' && pMethod[2] == 'T')
			return HTTP_METHOD_GET;
	}
	if(strlen(pMethod) == 4)
	{
		if(pMethod[0] == 'P' && pMethod[1] == 'O' && pMethod[2] == 'S' && pMethod[3] == 'T')
			return HTTP_METHOD_POST;
	}
	return HTTP_METHOD_GET;
}

int HttpRequestLine::GetMethod()
{
	return m_iMethod;
}
int HttpRequestLine::AppendString(char* pString, int iLength)
{
	char* pNewString = new char[m_iStringLength+iLength+1];
	if(m_pString)
		memcpy(pNewString, m_pString, m_iStringLength);
	memcpy(pNewString+m_iStringLength, pString, iLength);
	pNewString[m_iStringLength+iLength] = '\0';
	if(m_pString)
		delete m_pString;
	m_pString = pNewString;
	m_iStringLength += iLength;
	return 0;
}

