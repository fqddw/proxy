#include "NetUtils.h"
#include "CommonType.h"
#include "stdio.h"
#include "memory.h"
#include "TimeLib.h"
#include "DNSCache.h"
extern DNSCache* g_pDNSCache;
InetSocketAddress* NetUtils::GetHostByName(char* pHostName,int port)
{
	int ip = g_pDNSCache->getSaddrByHost(pHostName);
	if(ip != FALSE){
		return new InetSocketAddress(port, ip);
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
		printf("DNS time second %ld nsec %ld %s\n", sub.tv_sec, sub.tv_nsec, pHostName);
		if (ret != 0) {
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
				g_pDNSCache->AddRecord(pHostName, psa->sin_addr.s_addr);
				return new InetSocketAddress(port,psa->sin_addr.s_addr);
			}
		}
	}
	return NULL;
}
