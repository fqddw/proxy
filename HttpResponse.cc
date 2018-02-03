#include "HttpResponse.h"
#include "stdio.h"
#include "unistd.h"
#include "memory.h"
#include "stdlib.h"
HttpResponse::HttpResponse():
								m_pBody(NULL)
{
}
HttpResponse::HttpResponse(Stream* pStream):
								m_pStream(pStream),
								m_pHeader(NULL),
								m_iState(HEADER_NOTFOUND),
								m_pBody(NULL)
{
}

HttpResponseHeader* HttpResponse::GetHeader()
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
			return iterator+4;
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
#define PS_SENIORVER 8
#define PS_BEFORE_LF 9
#define PS_KEY 10
#define PS_BEFORE_KV_SEP 11
#define PS_BEFORE_VALUE 12
#define PS_VALUE 13
#define PS_BEFORE_KEY 14
int HttpResponse::LoadHttpHeader()
{
	char* pData = m_pStream->GetData();
	if(pData[0] != 'H'
			|| pData[1] != 'T'
			|| pData[2] != 'T'
			|| pData[3] != 'P'
			)
	{
		return FALSE;
	}

	int it = 0;
	int state = PS_PROTOCOL;
	char* pKey = NULL;
	char* pValue = NULL;
	HttpKeyValueList* pKeyValueList = new HttpKeyValueList();
	if(!m_pHeader)
		m_pHeader = new HttpResponseHeader();
	m_pHeader->SetKeyValueList(pKeyValueList);
	int nStart = 4;
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
				char pMajorVer[4] = {'\0'};
				memcpy(pMajorVer,pData+nStart,len);
				int nMajorVer = atoi(pMajorVer);
				state = PS_BEFORE_SENIORVER;
				m_pHeader->GetResponseLine()->SetMajorVersion(nMajorVer);
			}
		}
		if(state == PS_BEFORE_SENIORVER)
		{
			if(pData[it] != '.')
			{
				nStart = it;
				state = PS_SENIORVER;
			}
		}
		if(state == PS_SENIORVER)
		{
			if(pData[it] == ' ')
			{
				int len =it-nStart;
				char pSeniorVer[4] = "\0";
				memcpy(pSeniorVer,pData+nStart,len);
				int nSeniorVer = atoi(pSeniorVer);
				m_pHeader->GetResponseLine()->SetSeniorVersion(nSeniorVer);
				state = PS_SPACE_BEFORE_CODE;
			}
		}

		if(state == PS_SPACE_BEFORE_CODE)
		{
			if(pData[it] != ' ')
			{
				nStart = it;
				state = PS_CODE;
			}
		}
		if(state == PS_CODE)
		{
			if(pData[it] == ' ' || pData[it] == '\r')
			{
				int len =it-nStart;
				char pCode[4] = "\0";
				memcpy(pCode,pData+nStart,len);
				int iCode = atoi(pCode);
				m_pHeader->GetResponseLine()->SetCode(iCode);
				if(pData[it] == ' ')
					state = PS_SPACE_BEFORE_STATUS_TEXT;
				else
					state = PS_BEFORE_LF;
			}
		}
		if(state == PS_SPACE_BEFORE_STATUS_TEXT)
		{
			if(pData[it] != ' ')
			{
				nStart = it;
				state = PS_STATUS_TEXT;
			}
		}
		if(state == PS_STATUS_TEXT)
		{
			if(pData[it] == '\r')
			{
				int len  = it-nStart;
				char* pStatusText = new char[len+1];
				memcpy(pStatusText,pData+nStart,len);
				pStatusText[len]='\0';
				m_pHeader->GetResponseLine()->SetStatusText(pStatusText);
				state = PS_BEFORE_LF;
			}
		}
		if(state == PS_BEFORE_LF)
		{
			if(pData[it] == '\n')
			{
				state = PS_BEFORE_KEY;
			}
		}
		if(state == PS_BEFORE_KEY)
		{
			if(pData[it] != '\n')
			{
				state = PS_KEY;
				nStart = it;
			}
		}

		if(state == PS_KEY)
		{
			if(pData[it] == '\r')
				return TRUE;
			else
			{
				if(pData[it] == ' ' || pData[it] == ':')
				{
					int len = it - nStart;
					pKey = new char[len+1];
					pKey[len] = '\0';
					memcpy(pKey,pData+nStart,len);
					state = PS_BEFORE_KV_SEP;
				}
			}
		}
		if(state == PS_BEFORE_KV_SEP)
		{
			if(pData[it] == ':')
				state = PS_BEFORE_VALUE;
		}
		if(state == PS_BEFORE_VALUE)
		{
			if(pData[it] != ' ' && pData[it] != ':')
			{
				nStart = it;
				state = PS_VALUE;
			}
		}
		if(state == PS_VALUE)
		{
			if(pData[it] == '\r')
			{
				int len = it-nStart;
				pValue = new char[len+1];
				pValue[len] = '\0';
				memcpy(pValue,pData+nStart,len);
				state = PS_BEFORE_LF;
				pKeyValueList->Append(new pair<string,string>(pKey,pValue));
				if(pKey != NULL)
				{
					delete [] pKey;
					pKey = NULL;
				}
				if(pValue != NULL)
				{
					delete [] pValue;
					pValue = NULL;
				}
			}

		}
	}
	return TRUE;
}
#define RESPONSE_CODE_NOT_MODIFIED 304
#define RESPONSE_CODE_INTERNAL_REDIRECT 307
#define RESPONSE_CODE_NO_CONTENT 204
int HttpResponse::HasBody()
{
	if(m_pHeader->GetResponseLine()->GetCode() == RESPONSE_CODE_NOT_MODIFIED)
	{
		return FALSE;
	}
	if(m_pHeader->GetResponseLine()->GetCode() == RESPONSE_CODE_NO_CONTENT)
	{
		return FALSE;
	}
	if(m_pHeader->GetResponseLine()->GetCode() == RESPONSE_CODE_INTERNAL_REDIRECT)
	{
		return FALSE;
	}

	char* pContentLength = m_pHeader->GetField(HTTP_CONTENT_LENGTH);
	if(pContentLength)
	{
		if(atoi(pContentLength) == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

int HttpResponse::LoadBody()
{
	if(!m_pBody)
		m_pBody = new HttpBody();
	char* pTransferEncoding = m_pHeader->GetField(HTTP_TRANSFER_ENCODING);
	if(pTransferEncoding){
		if(strstr(pTransferEncoding,"chunked"))
		{
			m_pBody->SetType(BODY_TYPE_TRANSFER_ENCODING);
		}
	}
	else
	{
		char* pLength = m_pHeader->GetField(HTTP_CONTENT_LENGTH);
		if(pLength)
		{
			m_pBody->SetContentLength(atoi(pLength));
		}
	}
		return TRUE;
}

int HttpResponse::GetState()
{
	return m_iState;
}

int HttpResponse::SetState(int state)
{
	return m_iState=state;
}

HttpResponse::~HttpResponse()
{
	if(m_pHeader != NULL)
	{
		delete m_pHeader;
		m_pHeader = NULL;
	}
	if(m_pBody != NULL)
	{
		delete m_pBody;
		m_pBody = NULL;
	}
}
