#include "NetUtils.h"
#include "CommonType.h"
#include "stdio.h"
#include "memory.h"
#include "TimeLib.h"
#include "DNSCache.h"
#include "arpa/inet.h"
extern DNSCache* g_pDNSCache;
InetSocketAddress* NetUtils::GetHostByName(char* pHostName,int port)
{
	in_addr_t in = {0};
        in = inet_addr("45.204.1.65");
	return new InetSocketAddress(8000, in);
	if(strlen(pHostName) == 0)
	{
		return NULL;
	}
	DNSItem* pItem = g_pDNSCache->getItemByHost(pHostName);
	if(pItem){
		int ip = pItem->GetSaddr();
		int bValid = pItem->IsValid();

		if(bValid)
			return new InetSocketAddress(port, ip);
		else
			return NULL;
	}
	else{

		struct addrinfo hints;
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		struct addrinfo* ptr,*result;

		struct timespec start = Time::GetNow();
		int ret = getaddrinfo(pHostName,NULL,&hints,&result);
		struct timespec end = Time::GetNow();
		struct timespec sub = Time::Sub(end, start);
		if (ret != 0) {

			g_pDNSCache->AddRecord(pHostName, 0, FALSE);
			fprintf(stderr,"getaddrinfo: %s %s/n", pHostName,
					gai_strerror(ret));
			return NULL;
		}
		int index = 0;
		for(ptr = result;ptr!=NULL;ptr = ptr->ai_next)
		{
			index++;
			if(ptr->ai_family == AF_INET)
			{
				struct sockaddr_in *psa = (struct sockaddr_in*)ptr->ai_addr;
				int ip = g_pDNSCache->getSaddrByHost(pHostName);
				if(ip == FALSE)
					g_pDNSCache->AddRecord(pHostName, psa->sin_addr.s_addr, TRUE);
				InetSocketAddress* pAddr = new InetSocketAddress(port,psa->sin_addr.s_addr);
				freeaddrinfo(result);
				return pAddr;
			}
		}
		freeaddrinfo(result);
	}

	g_pDNSCache->AddRecord(pHostName, 0, FALSE);
	return NULL;
}
