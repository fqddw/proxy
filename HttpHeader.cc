#include "HttpHeader.h"
#include "CommonType.h"
#include <algorithm>
int HttpRequestHeader::SetRequestLine(HttpRequestLine* pHttpRequestLine)
{
	m_pRequestLine = pHttpRequestLine;
	return TRUE;
}
HttpRequestLine* HttpRequestHeader::GetRequestLine()
{
	return m_pRequestLine;
}
Stream* HttpHeader::ToHeader()
{
		return NULL;
}

int HttpRequestHeader::GetMethod()
{
	return
	m_pRequestLine->GetMethod();
}

int HttpHeader::SetKeyValueList(HttpKeyValueList* pKeyValueList)
{
	m_pKeyValueList = pKeyValueList;
	return TRUE;
}
Stream* HttpResponseHeader::ToHeader()
{
		return NULL;
}

HttpResponseLine* HttpResponseHeader::GetResponseLine()
{
	return m_pHttpResponseLine;
}

int HttpResponseHeader::SetResponseLine(HttpResponseLine* pResponseLine)
{
	m_pHttpResponseLine = pResponseLine;
}
typedef struct _FIELD_
{
	int index;
	const char* pText;
}FIELD;
char* HttpHeader::GetField(int iFieldIndex)
{
	static FIELD sFields[] = {
		{HTTP_CONTENT_LENGTH,"Content-Length"},
		{HTTP_SERVER,"Server"},
		{HTTP_CONNECTION,"Connection"},
		{HTTP_TRANSFER_ENCODING,"Transfer-Encoding"},
		{HTTP_COOKIE, "Cookie"},
	};
	int i=0;
	for(;i<sizeof(sFields)/sizeof(FIELD);i++)
	{
		if(iFieldIndex == sFields[i].index)
		{
			MemNode<pair<string,string>*>* pNode = m_pKeyValueList->GetHead();
			while(pNode!=NULL)
			{
				string lncaseStr = pNode->GetData()->first;
				transform(lncaseStr.begin(),lncaseStr.end(),lncaseStr.begin(),::tolower);
				string rncaseStr = sFields[i].pText;
				transform(rncaseStr.begin(),rncaseStr.end(),rncaseStr.begin(),::tolower);

				if(lncaseStr == rncaseStr)
				{
					return (char*)pNode->GetData()->second.c_str();
				}
				pNode = pNode->GetNext();
			}
		}
	}
	return NULL;
}

HttpKeyValueList* HttpHeader::GetKeyValueList()
{
	return m_pKeyValueList;
}

#include "string.h"
Stream* HttpRequestHeader::ToHeader()
{
	char* pCandString = m_pRequestLine->ToString();
	Stream* pStream = new Stream();
	pStream->Append(pCandString,strlen(pCandString));
	pStream->Append((char*)"\r\n",2);
	delete []pCandString;
	MemNode<pair<string,string>*>* pNode = GetKeyValueList()->GetHead();
	while(pNode != NULL)
	{
					char* pKey = (char*)pNode->GetData()->first.c_str();
					if(strstr(pKey, "Proxy-Connection"))
									pKey = (char*)"Connection";
					pStream->Append(pKey ,strlen(pKey));

		pStream->Append((char*)": ",2);
		pStream->Append((char*)pNode->GetData()->second.c_str(),pNode->GetData()->second.size());
		pStream->Append((char*)"\r\n",2);
		pNode = pNode->GetNext();
	}
	pStream->Append((char*)"\r\n",2);
	return pStream;
}

int HttpHeader::SetRawLength(int iRawLength)
{
	m_iRawLength = iRawLength;
	return TRUE;
}

int HttpHeader::GetRawLength()
{
	return m_iRawLength;
}

HttpHeader::~HttpHeader()
{
	if(m_pKeyValueList)
	{
		delete m_pKeyValueList;
		m_pKeyValueList = NULL;
	}
}

HttpRequestHeader::~HttpRequestHeader()
{
	if(m_pRequestLine != NULL)
	{
		delete m_pRequestLine;
		m_pRequestLine = NULL;
	}
}
HttpRequestHeader::HttpRequestHeader():m_pRequestLine(NULL)
{
}
HttpResponseHeader::HttpResponseHeader():m_pHttpResponseLine(NULL)
{
}
HttpResponseHeader::~HttpResponseHeader()
{
	if(m_pHttpResponseLine != NULL)
	{
		delete m_pHttpResponseLine;
		m_pHttpResponseLine = NULL;
	}
}
