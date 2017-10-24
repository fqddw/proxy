#include "AuthManager.h"
#include "Stream.h"
#include "Auth.h"
#include "stdlib.h"

extern AuthManager* g_pAuthManager;

Auth* AuthManager::GenerateAuthToken()
{
	Stream* pNonce = new Stream(NONCE_LENGTH);
	char* pNonceData = pNonce->GetData();
	int i = 0;
	do
	{
		for(i=0; i<pNonce->GetLength();i++)
		{
			sprintf(pNonceData+i, "%x",rand()%16);
		}
	}while(ExistsNonce(pNonce));
	Stream* pOpaque = new Stream(OPAQUE_LENGTH);
	char* pOpaqueData = pOpaque->GetData();
	do
	{
		for(i=0; i<pOpaque->GetLength();i++)
		{
			sprintf(pOpaqueData+i, "%x",rand()%16);
		}
	}while(ExistsOpaque(pOpaque));
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
