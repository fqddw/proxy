#include "HttpResponseLine.h"
#include "CommonType.h"

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

