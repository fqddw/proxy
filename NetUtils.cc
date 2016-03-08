#include "NetUtils.h"
#include "CommonType.h"
#include "stdio.h"
#include "memory.h"
InetSocketAddress* NetUtils::GetHostByName(char* pHostName,int port)
{
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* ptr,*result;
	int ret = getaddrinfo(pHostName,NULL,&hints,&result);
	if (ret != 0) {
		fprintf(stderr,"getaddrinfo: %s/n",
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
			return new InetSocketAddress(port,psa->sin_addr.s_addr);
		}
	}
	return NULL;
}
