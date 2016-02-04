#include "NetUtils.h"
InetSocketAddress* NetUtils::GetHostByName()
{
	char* pHostName = pHttpHeader->GetRequestLine()->GetUrl()->GetHost();
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_IP;
	struct addrinfo* ptr,*result;
	getaddrinfo(pHostName,NULL,&hints,&result);

	for(ptr = result;ptr!=NULL;ptr = ptr->ai_next)
	{
		if(ptr->ai_family == AF_INET)
		{
			struct sockaddr_in *psa = (struct sockaddr_in*)ptr->ai_addr;
			return new InetSocketAddress(psa->sin_addr.s_addr,htons(psa->sin_port));
		}
	}
}
