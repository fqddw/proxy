#include "HttpHeader.h"
#include "CommonType.h"
int HttpHeader::SetRequestLine(HttpRequestLine* pHttpRequestLine)
{
	m_pRequestLine = pHttpRequestLine;
	return TRUE;
}
HttpRequestLine* HttpHeader::GetRequestLine()
{
	return m_pRequestLine;
}
Stream* HttpHeader::ToHeader()
{
		return NULL;
}

char* HttpHeader::ToProxyHeader()
{
		return NULL;
}
int HttpHeader::SetUrl(HttpUrl* pUrl)
{
		m_pUrl = pUrl;
		return TRUE;
}
HttpUrl* HttpHeader::GetUrl()
{
		return m_pUrl;
}

int HttpHeader::GetMethod()
{
	return
	m_pRequestLine->GetMethod();
}

int HttpHeader::SetKeyValueList(HttpKeyValueList* pKeyValueList)
{
	m_pKeyValueList = pKeyValueList;
	return TRUE;
}
