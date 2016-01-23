#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "HttpRequestLine.h"

HttpRequestLine::HttpRequestLine():m_pString(0),m_iStringLength(0)
{
}
char* HttpRequestLine::ToString()
{
	char* pMethod = GetMethodString();
	char* pUrl = m_pUrl;
	char* pRequestString = new char[strlen(pMethod)+1+strlen(pUrl)+1+8+1];
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
	return (char*)"GET";
}

int HttpRequestLine::SetUrl(char* pUrl)
{
	m_pUrl = pUrl;
	return 0;
}

int HttpRequestLine::SetVersion(int major,int senior)
{
	m_iMajorVer = major;
	m_iSeniorVer = senior;
	return 0;
}
int HttpRequestLine::GetState()
{
	return m_State;
}
int HttpRequestLine::Parse()
{
	int state = 0;
	m_iState = state;
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
			state = 2;
			break;
		}
	}
	len = index-urlstart;

	char* pUrl = new char[len+1];
	memset(pUrl,'\0',len+1);
	memcpy(pUrl,pString+urlstart,len);
	m_pUrl = pUrl;
	state = 3;
	for(;index<m_iStringLength;index++){
		if(*(pString+index) != ' '){
			break;
		}
	}

	return 0;
}

int HttpRequestLine::GetMethodId(char* pMethod)
{
	
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

