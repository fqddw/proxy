#include "Auth.h"
#include "Stream.h"

Auth::Auth():m_pNonce(NULL),m_pOpaque(NULL),m_pRealm(NULL),m_pUser(NULL)
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

Stream* Auth::ToStream()
{
	Stream* pStream = new Stream();
	pStream->Append("Digest ");
	pStream->Append("realm=\"");
	pStream->Append(m_pRealm);
	pStream->Append("\",");
	pStream->Append("nonce=\"");
	pStream->Append(m_pNonce);
	pStream->Append("\",");
	pStream->Append("opaque=\"");
	pStream->Append(m_pOpaque);
	pStream->Append("\",");
	pStream->Append("qop=auth");
	return pStream;
}

void Auth::SetRealm(char* pRealm)
{
	if(!m_pRealm)
		m_pRealm = new Stream();
	m_pRealm->Append(pRealm);
}
Auth::~Auth()
{
	if(m_pNonce)
		delete m_pNonce;
	m_pNonce = NULL;
	if(m_pOpaque)
		delete m_pOpaque;
	m_pOpaque = NULL;
	if(m_pRealm)
		delete m_pRealm;
	m_pRealm = NULL;
}

void Auth::SetNonceAuthed(int bFlag)
{
	m_bNonceAuthed = bFlag;
}
void Auth::SetOpaqueAuthed(int bFlag)
{
	m_bOpaqueAuthed = bFlag;
}



void Auth::SetUser(User* pUser)
{
	m_pUser = pUser;
}


User* Auth::GetUser()
{
	return m_pUser;
}
