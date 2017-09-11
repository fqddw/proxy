#include "Stream.h"
#include "CommonType.h"
#include "string.h"
char* Stream::GetData()
{
	return m_pData;
}
int Stream::GetLength()
{
	return m_iLength;
}

int Stream::Append(char* pData,int length)
{
	if(!length)
		return FALSE;
	cs_->Enter();
	char* pNewData = new char[m_iLength+length];
	if(m_pData)
	{
		memcpy(pNewData,m_pData,m_iLength);
		delete [] m_pData;
		m_pData = NULL;
	}
	memcpy(pNewData+m_iLength,pData,length);
	m_pData = pNewData;
	m_iLength += length;
	cs_->Leave();
	return TRUE;
}
int Stream::Append(Stream* pStream)
{
	return Append(pStream->GetData(), pStream->GetLength());
}

Stream::Stream():m_pData(NULL),m_iLength(0),cs_(new CriticalSection())
{
}

Stream::Stream(int iLength):m_iLength(0),cs_(new CriticalSection())
{
				m_pData = new char[iLength];
				memset(m_pData,0,iLength);
}

Stream::~Stream()
{
	if(m_pData)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	m_iLength = 0;
	delete cs_;
}
#include "stdio.h"
int Stream::Sub(int offset)
{
	if(m_iLength == 0)
		return 0;
	cs_->Enter();
	int newLength = m_iLength - offset;
	if(newLength == 0)
	{
		if(m_pData != NULL)
		{
			delete [] m_pData;
		}
		m_iLength = 0;
		m_pData = NULL;
		cs_->Leave();
		return TRUE;
	}
	char* pNewData = new char[newLength];
	memcpy(pNewData, m_pData+offset, newLength);
	delete [] m_pData;
	m_pData = pNewData;
	m_iLength = newLength;
	cs_->Leave();
	return TRUE;
}

char* Stream::GetPartDataToString(int begin,int end)
{
	int length = end-begin;
	char* pReturnString = new char[length+1];
	pReturnString[length] = '\0';
	memcpy(pReturnString,m_pData+begin,length);
	return pReturnString;
}

Stream* Stream::GetPartStream(int begin,int end)
{
	int length = end-begin;
	char* pReturnString = new char[length];
	memcpy(pReturnString,m_pData+begin,length);
	Stream* pStream = new Stream();
	pStream->Append(pReturnString,length);
	delete []pReturnString;
	return pStream;

}

int Stream::Equal(Stream* pDest)
{
	if(pDest->m_iLength != m_iLength)
	{
		return FALSE;
	}

	int i = 0;
	for(;i<m_iLength;i++)
	{
		if(m_pData[i] != pDest->m_pData[i])
			return FALSE;
	}
	return TRUE;
}

int Stream::Equal(char* pDest)
{
	int len = strlen(pDest);
	if(len != m_iLength)
	{
		return FALSE;
	}

	int i = 0;
	for(;i<m_iLength;i++)
	{
		if(m_pData[i] != pDest[i])
			return FALSE;
	}
	return TRUE;
}
