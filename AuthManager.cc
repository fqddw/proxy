#include "AuthManager.h"
#include "Stream.h"
#include "Auth.h"
#include "stdlib.h"
#include "HttpHeader.h"
#include "string.h"
#include "memory.h"

extern AuthManager* g_pAuthManager;

Auth* AuthManager::GenerateAuthToken()
{
	cs_->Enter();
	Stream* pNonce = new Stream();
	char pNonceData[NONCE_LENGTH+1] = {0};
	int i = 0;
	do
	{
		for(i=0; i<NONCE_LENGTH;i++)
		{
			sprintf(pNonceData+i, "%x",rand()%16);
		}
		pNonce->Clear();
		pNonce->Append(pNonceData, NONCE_LENGTH);
	}while(ExistsNonce(pNonce));
	Stream* pOpaque = new Stream();
	char pOpaqueData[OPAQUE_LENGTH+1] = {0};
	do
	{
		for(i=0; i<OPAQUE_LENGTH;i++)
		{
			sprintf(pOpaqueData+i, "%x",rand()%16);
		}
		pOpaque->Clear();
		pOpaque->Append(pOpaqueData, OPAQUE_LENGTH);
	}while(ExistsOpaque(pOpaque));
	Auth* pAuth = new Auth();
	pAuth->SetNonce(pNonce);
	pAuth->SetOpaque(pOpaque);
	m_pAuthList->Append(pAuth);
	cs_->Leave();
	return pAuth;
}

int AuthManager::ExistsNonce(Stream* pNonce)
{
	MemNode<Auth*>* pAuthNode = m_pAuthList->GetHead();
	while(pAuthNode)
	{
		if(pAuthNode->GetData()->GetNonce()->Equal(pNonce))
		{
			return TRUE;
		}
		pAuthNode = pAuthNode->GetNext();
	}
	return FALSE;
}

int AuthManager::ExistsOpaque(Stream* pOpaque)
{
	MemNode<Auth*>* pAuthNode = m_pAuthList->GetHead();
	while(pAuthNode)
	{
		if(pAuthNode->GetData()->GetOpaque()->Equal(pOpaque))
		{
			return TRUE;
		}
		pAuthNode = pAuthNode->GetNext();
	}
	return FALSE;

}

AuthManager* AuthManager::getInstance()
{
	if(!m_pInstance)
	{
		m_pInstance = new AuthManager();
	}
	return m_pInstance;
}

AuthManager::AuthManager():m_pAuthList(new MemList<Auth*>()),cs_(new CriticalSection())
{
}

Auth* AuthManager::GetAuthByNonce(Stream* pNonce)
{
	MemNode<Auth*>* pAuthNode = m_pAuthList->GetHead();
	while(pAuthNode)
	{
		if(pAuthNode->GetData()->GetNonce()->Equal(pNonce))
		{
			return pAuthNode->GetData();
		}
		pAuthNode = pAuthNode->GetNext();
	}
	return NULL;
}


Stream* AuthManager::GetRequireAuthString()
{
	Auth* pAuth = AuthManager::getInstance()->GenerateAuthToken();

	pAuth->SetRealm(REALM_STRING);
	HttpResponseHeader* pAuthResp = new HttpResponseHeader();
	pAuthResp->GetResponseLine()->SetCode(407);
	pAuthResp->GetResponseLine()->SetMajorVersion(1);
	pAuthResp->GetResponseLine()->SetSeniorVersion(1);
	const char* chText = "Proxy Authentication Required";
	int statusTextLen = strlen(chText);
	char* pStatusText = new char[statusTextLen+1];
	pStatusText[statusTextLen] = '\0';
	memcpy(pStatusText, chText, statusTextLen);
	pAuthResp->GetResponseLine()->SetStatusText(pStatusText);
	pAuthResp->SetKeyValueList(new HttpKeyValueList());
	Stream* pAuthStream = pAuth->ToStream();
	pAuthResp->AppendHeader((char*)"Proxy-Authenticate", 18, pAuthStream->GetData(), pAuthStream->GetLength());
	pAuthResp->AppendHeader((char*)"Content-Length", 14, (char*)"0", 1);
	Stream* pAuthRespStream = pAuthResp->ToHeader();
	//printf("%s", pAuthRespStream->GetData());
	delete pAuthResp;
	//delete pAuth;
	delete pAuthStream;
	return pAuthRespStream;
}
