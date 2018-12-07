#include "MemStore.h"

StoreItem::StoreItem()
{
	m_pHost = NULL;
	m_pRequestURL = NULL;
	m_pResponse = NULL;
}
StoreItem::StoreItem(Stream* pHost, Stream* pRequestURL)
{
	m_pHost = NULL;
	m_pRequestURL = NULL;
	m_pResponse = NULL;

	SetHost(pHost);
	SetRequestURL(pRequestURL);
	m_pResponse = new Stream();
}

Stream* StoreItem::GetHost()
{
	return m_pHost;
}

Stream* StoreItem::GetRequestURL()
{
	return m_pRequestURL;
}

void StoreItem::SetHost(Stream* pHost)
{
	if(m_pHost != NULL)
		m_pHost->Clear();
	m_pHost = new Stream();
	m_pHost->Append(pHost);
}

void StoreItem::SetRequestURL(Stream* pRequestURL)
{
	if(m_pRequestURL != NULL)
		m_pRequestURL->Clear();
	m_pRequestURL = new Stream();
	m_pRequestURL->Append(pRequestURL);
}

void StoreItem::AppendResponse(Stream* pStream)
{
	m_pResponse->Append(pStream);
}

Stream* StoreItem::GetResponse()
{
	return m_pResponse;
}

MemStore::MemStore()
{
	m_pList = new MEMSTORE;
}

StoreItem* MemStore::AppendItem(StoreItem* pItem)
{
	m_pList->Append(pItem);
	return pItem;
}

MemStore* MemStore::getInstance()
{
	if(!g_pInstance)
		g_pInstance = new MemStore();
	return g_pInstance;
}

StoreItem* MemStore::GetByHostAndUrl(Stream* pHost, Stream* pRequestURL)
{
	m_pList->Lock();
	MemNode<StoreItem*>* pNode = m_pList->GetHead();
	if(!pNode)
	{
		m_pList->Unlock();
		return NULL;
	}
	for(;pNode!=NULL;pNode = pNode->GetNext())
	{
		if(pNode->GetData()->GetHost()->Equal(pHost) && pNode->GetData()->GetRequestURL()->Equal(pRequestURL))
		{
			m_pList->Unlock();
			return pNode->GetData();
		}
	}
	m_pList->Unlock();
	return NULL;
}
