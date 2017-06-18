#include "DNSCache.h"
#include "string.h"
#include "stdlib.h"

int DNSItem::GetSaddr()
{
	return m_iSaddr;
}

char* DNSItem::GetHostName()
{
	return m_pHostName;
}

void DNSItem::SetSaddr(int iSaddr)
{
	m_iSaddr = iSaddr;
}

void DNSItem::SetHostName(char* pHost)
{
	int len = strlen(pHost);
	char* dst = (char*)malloc(len+1);
	dst[len] = '\0';
	memcpy(dst, pHost, len);
	m_pHostName = dst;
}

DNSCache::DNSCache():m_pCacheData(new MemList<DNSItem*>)
{
}
int DNSCache::getSaddrByHost(char* pHost)
{
	MemNode<DNSItem*>* pItem = m_pCacheData->GetHead();
	while(pItem) {
		if(strlen(pHost) == strlen(pItem->GetData()->GetHostName())) {
			if(strstr(pHost, pItem->GetData()->GetHostName()))
				return pItem->GetData()->GetSaddr();
		}
		pItem = pItem->GetNext();
	}
	return FALSE;
}

int DNSCache::AddRecord(char* pHost, int iSaddr)
{
	DNSItem* pDNSItem = new DNSItem;
	pDNSItem->SetSaddr(iSaddr);
	pDNSItem->SetHostName(pHost);
	m_pCacheData->Append(pDNSItem);
	return TRUE;
}

MemList<DNSItem*>* DNSCache::GetList()
{
	        return m_pCacheData;
}

