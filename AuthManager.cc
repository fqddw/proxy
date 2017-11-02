#include "AuthManager.h"
#include "Stream.h"
#include "Auth.h"
#include "stdlib.h"

extern AuthManager* g_pAuthManager;

Auth* AuthManager::GenerateAuthToken()
{
	Stream* pNonce = new Stream();
	char pNonceData[NONCE_LENGTH+1] = {0};
	int i = 0;
	do
	{
		for(i=0; i<NONCE_LENGTH;i++)
		{
			sprintf(pNonceData+i, "%x",rand()%16);
		}
	}while(ExistsNonce(pNonce));
	pNonce->Append(pNonceData, NONCE_LENGTH);
	Stream* pOpaque = new Stream();
	char pOpaqueData[OPAQUE_LENGTH+1] = {0};
	do
	{
		for(i=0; i<OPAQUE_LENGTH;i++)
		{
			sprintf(pOpaqueData+i, "%x",rand()%16);
		}
	}while(ExistsOpaque(pOpaque));
	pOpaque->Append(pOpaqueData, OPAQUE_LENGTH);
	Auth* pAuth = new Auth();
	pAuth->SetNonce(pNonce);
	pAuth->SetOpaque(pOpaque);
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

AuthManager::AuthManager():m_pAuthList(new MemList<Auth*>())
{
}

