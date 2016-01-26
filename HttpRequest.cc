#include "HttpRequest.h"
#include "CommonType.h"
#include "stdio.h"
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
