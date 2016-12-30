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
Stream::Stream():m_pData(NULL),m_iLength(0),cs_(new CriticalSection())
{
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
