#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "HttpRequestLine.h"
#include "CommonType.h"
typedef struct __STHttpMethod
{
	int id;
	char* name;
}HttpMethod;
HttpMethod methods[] = {
	{HTTP_METHOD_GET, (char*)"GET"},
	{HTTP_METHOD_POST, (char*)"POST"},
	{HTTP_METHOD_OPTIONS, (char*)"OPTIONS"},
	{HTTP_METHOD_PUT, (char*)"PUT"},
	{HTTP_METHOD_DELETE, (char*)"DELETE"},
	{HTTP_METHOD_CONNECT, (char*)"CONNECT"},
	{HTTP_METHOD_HEAD, (char*)"HEAD"},
	{HTTP_METHOD_TRACE, (char*)"TRACE"}
};
HttpRequestLine::HttpRequestLine()
{
								m_pString = (0);
								m_iStringLength = (0);
								m_pUrl = (NULL);
								m_pMethodStream = (new Stream());

}
HttpUrl* HttpRequestLine::GetUrl()
{
	return m_pUrl;
}
char* HttpRequestLine::ToString()
{
	char* pMethod = GetMethodString();
	char* pUrl = m_pUrl->ToString();
	int reqLen = strlen(pMethod)+1+strlen(pUrl)+1+8+1;
	char* pRequestString = new char[reqLen];
	pRequestString[reqLen-1] = '\0';
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
Stream* HttpRequestLine::GetMethodStream()
{
	return m_pMethodStream;
}

char* HttpRequestLine::GetMethodString()
{
	unsigned int i=0;
	for(;i<sizeof(methods);i++)
	{
		if(m_iMethod == (methods+i)->id)
		{
			return (methods+i)->name;
		}
	}
	return NULL;
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
	m_pMethodStream->Append(pMethod, len);
	delete []pMethod;
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
	delete []pUrl;
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
	else
		return FALSE;
	return TRUE;
}

int HttpRequestLine::GetMethodId(char* pMethod)
{
	unsigned int i = 0;
	for(;i<sizeof(methods)/sizeof(methods[0]);i++)
	{
		if(strlen((methods+i)->name) == strlen(pMethod))
		{
			if(strstr(pMethod, (methods+i)->name))
				return (methods+i)->id;
		}
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

HttpRequestLine::~HttpRequestLine()
{
				if(m_pUrl)
								delete m_pUrl;
				if(m_pString)
								delete []m_pString;
				m_pUrl = NULL;
				m_pString = NULL;
				delete m_pMethodStream;
}
