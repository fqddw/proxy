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
Stream* HttpRequestHeader::ToHeader()
{
		return NULL;
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
