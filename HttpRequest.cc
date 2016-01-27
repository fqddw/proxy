#include "HttpRequest.h"
#include "CommonType.h"
#include "stdio.h"
#include "string"
#include "HttpUrl.h"
using namespace std;
HttpRequest::HttpRequest()
{
}

HttpRequest::HttpRequest(Stream* pStream)
{
	m_pStream = pStream;
}

int HttpRequest::IsHeaderEnd()
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
HttpHeader* HttpRequest::GetHeader()
{
	return m_pHttpHeader;
}
int HttpRequest::LoadHttpHeader()
{
	HttpHeader* pHeader = new HttpHeader();
	m_pHttpHeader = pHeader;
	string String_Stream;
	String_Stream.assign(m_pStream->GetData(),m_pStream->GetLength());
	string strHeaderString = String_Stream.substr(0,String_Stream.find("\r\n\r\n"));
	string stringHttpRequestLine = strHeaderString.substr(0,strHeaderString.find("\r\n"));
	HttpRequestLine* pLine = new HttpRequestLine();
	pLine->AppendString((char*)stringHttpRequestLine.data(),stringHttpRequestLine.size());
	pLine->Parse();
	pHeader->SetRequestLine(pLine);
	HttpUrl* pHttpUrl = pLine->GetUrl();
	pHttpUrl->Parse();
	return TRUE;
}
