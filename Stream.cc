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
	char* pNewData = new char[m_iLength+length];
	if(m_pData)
	{
		memcpy(pNewData,m_pData,m_iLength);
		delete m_pData;
	}
	memcpy(pNewData+m_iLength,pData,length);
	m_pData = pNewData;
	m_iLength += length;
	return TRUE;
}
Stream::Stream():m_pData(NULL),m_iLength(0)
{
}

Stream::~Stream()
{
	if(m_pData)
	{
		delete m_pData;
		m_pData = NULL;
	}
	m_iLength = 0;
}
int Stream::Sub(int offset)
{
	int newLength = m_iLength - offset;
	char* pNewData = new char[newLength];
	memcpy(pNewData, m_pData+offset, newLength);
	delete m_pData;
	m_pData = pNewData;
	return TRUE;
}
