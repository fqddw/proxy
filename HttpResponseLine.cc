#include "HttpResponseLine.h"
#include "CommonType.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"

HttpResponseLine::HttpResponseLine():m_pStatusText(NULL)
{
}

HttpResponseLine::~HttpResponseLine()
{
				if(m_pStatusText)
								delete [] m_pStatusText;
				m_pStatusText = NULL;
}

int HttpResponseLine::SetCode(int iCode)
{
	m_iCode = iCode;
	return iCode;
}

int HttpResponseLine::GetCode()
{
	return m_iCode;
}

int HttpResponseLine::SetStatusText(char* pText)
{
	m_pStatusText = pText;
	return TRUE;
}

char* HttpResponseLine::GetStatusText()
{
	return m_pStatusText;
}

int HttpResponseLine::GetMajorVersion()
{
	return m_iMajorVersion;
}

int HttpResponseLine::GetSeniorVersion()
{
	return m_iSeniorVersion;
}

int HttpResponseLine::SetMajorVersion(int iMajorVersion)
{
	m_iMajorVersion = iMajorVersion;
	return TRUE;
}

int HttpResponseLine::SetSeniorVersion(int iSeniorVersion)
{
	m_iSeniorVersion = iSeniorVersion;
	return TRUE;
}


char* HttpResponseLine::ToString()
{
	char* pFormat = (char*)"HTTP/%d.%d %d %s";
	char buffer[1024] = {0};
	sprintf(buffer, pFormat, m_iMajorVersion, m_iSeniorVersion,  m_iCode, m_pStatusText);
	int len = strlen(buffer);
	char* pString = new char[len+1];
	pString[len] = '\0';
	memcpy(pString, buffer, len);
	return pString;
}
