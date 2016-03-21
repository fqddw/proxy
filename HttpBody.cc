#include "HttpBody.h"
#include "CommonType.h"
#include "stdlib.h"
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
			m_iCurLength += pStream->GetLength();
			return TRUE;
		}
		else
			return FALSE;
	}
	if(m_iType == BODY_TYPE_TRANSFER_ENCODING)
	{
		Parse(pStream);
		if(GetLastChunkLength() == 0)
			return TRUE;
		else
			return FALSE;
	}
		return TRUE;
}
HttpBody::HttpBody():m_iType(BODY_TYPE_CONTENT_LENGTH),m_iCurChunkLength(0),m_iIsEnd(FALSE),m_iChunkState(CS_IN_BEGIN_CRLF),m_iCurInChunkLength(0),m_iOffset(0),m_pLengthStream(NULL),m_iLength(0),m_iCurLength(0)
{
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
	char* pCrlf = "\r\n";
	while(offset < pStream->GetLength())
	{
		if(m_iChunkState == CS_IN_CHUNK)
		{
			if(pStream->GetLength() + m_iCurInChunkLength < m_iCurChunkLength)
			{
				m_iCurInChunkLength += pStream->GetLength();
				offset += pStream->GetLength();
				break;
			}
			else
			{
				offset += (m_iCurChunkLength-m_iCurInChunkLength);
				m_iChunkState == CS_IN_BEGIN_CRLF;
				begin = offset;
			}
		}
		if(m_iChunkState == CS_IN_BEGIN_CRLF)
		{
			if(offset + 2 > pStream->GetLength())
			{
				m_iOffset = pStream->GetLength()-1-offset;
				break;
			}
			else
			{
				offset += 2;
				m_iChunkState = CS_IN_LENGTH;
				begin = offset;
			}
		}
		if(m_iChunkState == CS_IN_LENGTH)
		{
			int offset = 0;
			for(;offset<pStream->GetLength();offset++)
			{
				if(pData[offset] == '\r')
				{
					char* pLength = pStream->GetPartDataToString(begin,offset);
					m_iCurChunkLength = atoi(pLength);
					delete [] pLength;
					m_iChunkState = CS_IN_END_CRLF;
					if(offset == pStream->GetLength()-1)
					{
						m_iOffset = 1;
					}
					else
					{
						if(pData[offset+1] == '\n')
						{
							m_iChunkState = CS_IN_CHUNK;
							m_iOffset = 0;
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
			}
		}
		if(m_iChunkState == CS_IN_END_CRLF)
		{
			int lengthFlag = pStream->GetLength()-(2-m_iOffset);
			int insectLength =lengthFlag?pStream->GetLength():(2-m_iOffset);
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
}

int HttpBody::SetContentLength(int length)
{
	m_iLength = length;
	return TRUE;
}
