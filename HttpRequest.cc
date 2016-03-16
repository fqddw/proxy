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
	int posHeaderEnd = String_Stream.find("\r\n\r\n");
	string strHeaderString = String_Stream.substr(0,posHeaderEnd);
	int nLineEnd = strHeaderString.find("\r\n");
	string stringHttpRequestLine = strHeaderString.substr(0,nLineEnd);
	HttpRequestLine* pLine = new HttpRequestLine();
	pLine->AppendString((char*)stringHttpRequestLine.data(),stringHttpRequestLine.size());
	pLine->Parse();
	pHeader->SetRequestLine(pLine);
	HttpUrl* pHttpUrl = pLine->GetUrl();
	pHttpUrl->Parse();
	pHeader->SetUrl(pHttpUrl);

	HttpKeyValueList* pKeyValueList = new HttpKeyValueList();
	pHeader->SetKeyValueList(pKeyValueList);
	int curPos = String_Stream.find("\r\n") + 2;
	while(curPos < posHeaderEnd)
	{
		curPos = String_Stream.find("\r\n",curPos);
		if(curPos == posHeaderEnd)
		{
			return TRUE;
		}
		int start = curPos + 2;
		int nKeyPosStart = start;
		int nKeyPosEnd = String_Stream.find(":",nKeyPosStart);
		string key = String_Stream.substr(nKeyPosStart,nKeyPosEnd-nKeyPosStart);
		start = nKeyPosEnd+1;
		int nValueStart = String_Stream.find_first_not_of(" ",start);
		int nValueEnd = String_Stream.find("\r\n",nValueStart);
		string value = String_Stream.substr(nValueStart,nValueEnd-nValueStart);
		pair<string,string> *keyvalue = new pair<string,string>(key,value);
		pKeyValueList->Append(keyvalue);
		curPos = curPos + 2;
	}
	
	return TRUE;
}
HttpBody* HttpRequest::GetBody()
{
		return m_pHttpBody;
}
int HttpRequest::LoadBody()
{
		return TRUE;
}
int HttpRequest::HasBody()
{
	int nMethod = m_pHttpHeader->GetMethod();
	if(nMethod == HTTP_METHOD_GET)
	{
		return FALSE;
	}

	if(nMethod == HTTP_METHOD_POST)
	{
		return TRUE;
	}

	return FALSE;
}
