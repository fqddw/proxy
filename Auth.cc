#include "Auth.h"
#include "Stream.h"

Auth::Auth():m_pNonce(NULL),m_pOpaque(NULL)
{
}


void Auth::SetNonce(Stream* pStream)
{
	m_pNonce = pStream;
}


void Auth::SetOpaque(Stream* pStream)
{
	m_pOpaque = pStream;
}

Stream* Auth::GetNonce()
{
	return m_pNonce;
}

Stream* Auth::GetOpaque()
{
	return m_pOpaque;
}
