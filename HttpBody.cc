#include "HttpBody.h"
#include "CommonType.h"
#include "stdlib.h"
#include "stdio.h"
Stream* HttpBody::ToStream(Stream* pStream)
{
		Stream* pLocalStream = new Stream();
		pLocalStream->Append(pStream->GetData(),pStream->GetLength());
		return pLocalStream;
}
int HttpBody::IsEnd()
{
	return m_iIsEnd;
}
int HttpBody::IsEnd(Stream* pStream)
{
	if(m_iType == BODY_TYPE_CONTENT_LENGTH)
	{
		if(pStream->GetLength()+m_iCurLength == m_iLength)
		{
			m_iIsEnd = TRUE;
			return TRUE;
		}
		else
		{
			m_iCurLength += pStream->GetLength();
			return FALSE;
		}
	}
	if(m_iType == BODY_TYPE_TRANSFER_ENCODING)
	{
		Parse(pStream);
		if(IsEnd() == TRUE)
		{
			return TRUE;
		}
		else
			return FALSE;
	}
		return TRUE;
}
HttpBody::HttpBody()
{
								m_iType = (BODY_TYPE_CONTENT_LENGTH);
								m_iCurChunkLength = (0);
								m_iIsEnd = (FALSE);
								m_iChunkState = (CS_IN_LENGTH);
								m_iCurInChunkLength = (0);
								m_iOffset = (0);
								m_pLengthStream = (new Stream());
								m_iLength = (0);
								m_iCurLength = (0);

}

int HttpBody::SetType(int type)
{
	m_iType = type;
	return TRUE;
}

int HttpBody::GetLastChunkLength()
{
	return m_iCurChunkLength;
}
int HttpBody::Parse(Stream* pStream)
{
	char* pData = pStream->GetData();
	int offset = 0;
	int begin = 0;
	char* pCrlf = (char*)"\r\n";
	while(offset < pStream->GetLength())
	{
		if(m_iChunkState == CS_IN_CHUNK)
		{
			if(pStream->GetLength()- offset  + m_iCurInChunkLength < m_iCurChunkLength)
			{
				m_iCurInChunkLength += (pStream->GetLength()-offset);
				break;
			}
			else
			{
				offset += (m_iCurChunkLength-m_iCurInChunkLength);
				m_iChunkState = CS_IN_BEGIN_CRLF;
				m_iCurInChunkLength = 0;
				m_iCurChunkLength = 0;
				m_iOffset = 0;
				begin = offset;
			}
		}
		if(m_iChunkState == CS_IN_BEGIN_CRLF)
		{
			if(offset + (2-m_iOffset) > pStream->GetLength())
			{
				m_iOffset = 2-pStream->GetLength()+offset;
				break;
			}
			else
				if(offset + (2-m_iOffset) == pStream->GetLength())
			{
				offset += (2-m_iOffset);
				m_iOffset = 0;
				m_iChunkState = CS_IN_LENGTH;
				begin = offset;
				break;
			}
				else
				{
			if(m_iOffset == 0)
				offset += (2-m_iOffset);
			else
				offset += m_iOffset;
				m_iOffset = 0;
				m_iChunkState = CS_IN_LENGTH;
				begin = offset;
			}

		}
		if(m_iChunkState == CS_IN_LENGTH)
		{
			for(;offset<pStream->GetLength();offset++)
			{
				if(pData[offset] == '\r')
				{
					Stream* pPartData = pStream->GetPartStream(begin,offset);
					if(pPartData)
					{
						m_pLengthStream->Append(pPartData->GetData(),pPartData->GetLength());
						delete pPartData;
					}
					m_pLengthStream->Append((char*)"\0",1);
					char* pLength = m_pLengthStream->GetPartDataToString(0,m_pLengthStream->GetLength());
					int chunkLength = 0;
					sscanf(pLength,"%x",&chunkLength);
					delete m_pLengthStream;
					m_pLengthStream = new Stream();

					m_iCurChunkLength = chunkLength;
					m_iCurInChunkLength = 0;
					delete [] pLength;
					m_iChunkState = CS_IN_END_CRLF;
					if(chunkLength == 0)
					{
						m_iIsEnd = TRUE;
						return TRUE;
					}
					if(offset == pStream->GetLength()-1)
					{
						m_iOffset = 1;
						return TRUE;
					}
					else
					{
						if(pData[offset+1] == '\n')
						{
							m_iChunkState = CS_IN_CHUNK;
							m_iOffset = 0;
							offset += 2;
							begin = offset;
						}
						else
							return FALSE;
					}
					break;
				}
			}
			if(m_iChunkState == CS_IN_LENGTH)
			{
				m_pLengthStream->Append(pData+begin,offset-begin);
				break;
			}
		}
		if(m_iChunkState == CS_IN_END_CRLF)
		{
			int lengthFlag = pStream->GetLength()-(2+offset-m_iOffset);
			int insectLength =lengthFlag?(2-m_iOffset):pStream->GetLength();
			int i = m_iOffset;
			int flag = FALSE;
			for(;i< insectLength; i++)
			{
				if(pData[offset] != pCrlf[i])
				{
					flag = TRUE;
				}
			}
			if(flag)
			{
				return FALSE;
			}
			offset += insectLength;
			if(lengthFlag)
			{
				m_iOffset = 0;
				m_iChunkState = CS_IN_CHUNK;
			}
			else
			{
				return -1;
			}
		}
	}
	return TRUE;
}

int HttpBody::SetContentLength(int length)
{
	m_iLength = length;
	return TRUE;
}

HttpBody::~HttpBody()
{
	delete m_pLengthStream;
}
