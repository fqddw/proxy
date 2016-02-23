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
char* HttpHeader::ToHeader()
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
