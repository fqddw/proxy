#include "HttpHeader.h"
#include "CommonType.h"
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

int HttpRequestHeader::SetUrl(HttpUrl* pUrl)
{
		m_pUrl = pUrl;
		return TRUE;
}
HttpUrl* HttpRequestHeader::GetUrl()
{
		return m_pUrl;
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
		{HTTP_CONNECTION,"Connection"}
	};
	int i=0;
	for(;i<sizeof(sFields)/sizeof(FIELD);i++)
	{
		if(iFieldIndex == sFields[i].index)
		{
			MemNode<pair<string,string>*>* pNode = m_pKeyValueList->GetHead();
			while(pNode!=NULL)
			{
				if(pNode->GetData()->first == string(sFields[i].pText))
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
	MemNode<pair<string,string>*>* pNode = GetKeyValueList()->GetHead();
	while(pNode != NULL)
	{
		pStream->Append((char*)pNode->GetData()->first.c_str(),pNode->GetData()->first.size());
		pStream->Append((char*)": ",2);
		pStream->Append((char*)pNode->GetData()->second.c_str(),pNode->GetData()->second.size());
		pStream->Append((char*)"\r\n",2);
		pNode = pNode->GetNext();
	}
	pStream->Append((char*)"\r\n",2);
	return pStream;
}
