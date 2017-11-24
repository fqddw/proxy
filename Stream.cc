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
	int newLength = m_iLength+length;
	char* pNewData = new char[newLength];
	if(m_pData)
	{
		memcpy(pNewData,m_pData,m_iLength);
		delete []  m_pData;
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
int Stream::Append(char* pString)
{
	return Append(pString, strlen(pString));
}
int Stream::Append(const char* pString)
{
	return Append((char*)pString, strlen(pString));
}

Stream::Stream():m_pData(NULL),m_iLength(0),cs_(new CriticalSection())
{
}

Stream::Stream(int iLength):m_iLength(iLength),cs_(new CriticalSection())
{
				m_pData = new char[iLength];
				memset(m_pData,0,iLength);
}

Stream::~Stream()
{
	if(m_pData)
	{
		delete []  m_pData;
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
			delete []  m_pData;
		}
		m_iLength = 0;
		m_pData = NULL;
		cs_->Leave();
		return TRUE;
	}
	char* pNewData = new char[newLength];
	memcpy(pNewData, m_pData+offset, newLength);
	delete []  m_pData;
	m_pData = pNewData;
	m_iLength = newLength;
	cs_->Leave();
	return TRUE;
}

char* Stream::GetPartDataToString(int begin,int end)
{
	cs_->Enter();
	int length = end-begin;
	char* pReturnString = new char[length+1];
	pReturnString[length] = '\0';
	memcpy(pReturnString,m_pData+begin,length);
	cs_->Leave();
	return pReturnString;
}

Stream* Stream::GetPartStream(int begin,int end)
{
	cs_->Enter();
	int length = end-begin;
	if(length == 0)
	{
		cs_->Leave();
		return NULL;
	}
	char* pReturnString = new char[length];
	memcpy(pReturnString,m_pData+begin,length);
	Stream* pStream = new Stream();
	pStream->Append(pReturnString,length);
	delete []  pReturnString;
	cs_->Leave();
	return pStream;

}

int Stream::Equal(Stream* pDest)
{
	Lock();
	pDest->Lock();
	if(pDest->m_iLength != m_iLength)
	{
		pDest->Unlock();
		Unlock();
		return FALSE;
	}

	int i = 0;
	for(;i<m_iLength;i++)
	{
		if(m_pData[i] != pDest->m_pData[i])
		{
			pDest->Unlock();
			Unlock();
			return FALSE;
		}
	}
	pDest->Unlock();
	Unlock();

	return TRUE;
}

int Stream::Equal(char* pDest)
{
	cs_->Enter();
	int len = strlen(pDest);
	if(len != m_iLength)
	{
		cs_->Leave();
		return FALSE;
	}

	int i = 0;
	for(;i<m_iLength;i++)
	{
		if(m_pData[i] != pDest[i])
		{
			cs_->Leave();
			return FALSE;
		}
	}
	cs_->Leave();
	return TRUE;
}

void Stream::Clear()
{
	cs_->Enter();
	if(m_pData)
		delete []m_pData;
	m_pData = NULL;
	m_iLength = 0;
	cs_->Leave();
}

Stream::Stream(char* pData):cs_(new CriticalSection())
{
	int len = strlen(pData);
	m_iLength = len;
	m_pData = new char[len];
	memcpy(m_pData, pData, len);
}

Stream::Stream(const char* pData):cs_(new CriticalSection())
{
	int len = strlen(pData);
	m_iLength = len;
	m_pData = new char[len];
	memcpy(m_pData, pData, len);
}

void Stream::Lock()
{
	cs_->Enter();
}

void Stream::Unlock()
{
	cs_->Leave();
}
