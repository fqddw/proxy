#include "HttpResponse.h"
HttpHeader* HttpResponse::GetHeader()
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
			return TRUE;
		}
	}
	return FALSE;
}

HttpBody* HttpResponse::GetBody()
{
	return m_pBody;
}

int HttpResponse::LoadHttpHeader()
{
	return 0;
}

int HttpResponse::HasBody()
{}

int HttpResponse::LoadBody()
{}
