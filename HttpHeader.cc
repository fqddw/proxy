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

