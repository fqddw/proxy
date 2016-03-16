#include "HttpResponse.h"
#include "stdio.h"
#include "unistd.h"
#include "memory.h"
#include "stdlib.h"
HttpResponse::HttpResponse()
{
}
HttpResponse::HttpResponse(Stream* pStream):m_pStream(pStream),m_pHeader(NULL)
{
}

HttpHeader* HttpResponse::GetHeader()
{
	return m_pHeader;
}
int HttpResponse::IsHeaderEnd()
{
	int iterator = 0;
	char* pData = m_pStream->GetData();
	for(;iterator<=m_pStream->GetLength() - 4;iterator++)
	{
		if(pData[iterator] == '\r' && pData[iterator+1] == '\n' && pData[iterator+2] == '\r' && pData[iterator+3] == '\n')
		{
			return TRUE;
		}
	}
	return FALSE;
}

HttpBody* HttpResponse::GetBody()
{
	return m_pBody;
}
#define PS_PROTOCOL 0
#define PS_SPACE_BEFORE_CODE 1
#define PS_CODE 2
#define PS_SPACE_BEFORE_STATUS_TEXT 3
#define PS_STATUS_TEXT 4
#define PS_MAJORVER 5
#define PS_BEFORE_MAJORVER 6
#define PS_BEFORE_SENIORVER 7
int HttpResponse::LoadHttpHeader()
{
	char* pData = m_pStream->GetData();
	int it = 0;
	int state = PS_PROTOCOL;
	if(!m_pHeader)
		m_pHeader = new HttpHeader();
	int nStart = 0;
	while(1)
	{
		for(;it <m_pStream->GetLength();it++)
		{
			if(state == PS_PROTOCOL)
			{
				if(pData[it] == '/')
				{
					state = PS_BEFORE_MAJORVER;
				}
			}

			if(state == PS_BEFORE_MAJORVER)
			{
				if(pData[it] != '/')
				{
					nStart = it;
					state = PS_MAJORVER;
				}
			}

			if(state == PS_MAJORVER)
			{
				if(pData[it] == '.')
				{
					int len = it-nStart;
					char* pMajorVer = new char[len+1];
					pMajorVer[len]='\0';
					memcpy(pMajorVer,pData+nStart,len);
					int nMajorVer = atoi(pMajorVer);
					state = PS_BEFORE_SENIORVER;
				}
			}
			if(state == PS_BEFORE_SENIORVER)
			{
				if(pData[it] != ' ')
				{
					char* 
					state = PS_SPACE_BEFORE_CODE;
				}
			}

			if(state == PS_SPACE_BEFORE_CODE)
			{
				if(pData[it] != ' ')
				{
					state = PS_CODE;
				}
			}
			if(state == PS_CODE)
			{
				if(pData[it] == ' ')
				{
					state = PS_SPACE_BEFORE_STATUS_TEXT;
				}
				else
				{

				}
			}
			if(state == PS_SPACE_BEFORE_STATUS_TEXT)
			{
				if(pData[it] != ' ')
				{
					state = PS_STATUS_TEXT;
				}
			}
			if(state == PS_STATUS_TEXT)
			{
				if(pData[it] == '\r')
				{
				}
			}
		}
	}
	return 0;
}

int HttpResponse::HasBody()
{}

int HttpResponse::LoadBody()
{}
