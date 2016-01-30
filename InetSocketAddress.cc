#include "InetSocketAddress.h"
#include "CommonType.h"
InetSocketAddress::InetSocketAddress(int port,in_addr_t addr)
{
	m_ipv4 = addr;
	m_iPort = port;
}

InetSocketAddress::InetSocketAddress()
{
}

void InetSocketAddress::SetPort(int port)
{
	m_iPort = port;
}

struct sockaddr_in InetSocketAddress::ToSockAddrIn()
{
	struct sockaddr_in sa_i = {0};
	sa_i.sin_family = AF_INET;
	sa_i.sin_port = htons(m_iPort);
	sa_i.sin_addr.s_addr = m_ipv4;
	return sa_i;
}
int InetSocketAddress::ToIpV4Short()
{
	int ipv4 = ToSockAddrIn().sin_addr.s_addr;
	return ipv4;
}
struct sockaddr InetSocketAddress::ToSockAddr()
{
	struct sockaddr_in sa = ToSockAddrIn();
	return *(struct sockaddr*)&sa;
}


int InetSocketAddress::Size()
{
	return sizeof(struct sockaddr);
}

int InetSocketAddress::Equal(InetSocketAddress* pAddr)
{
	if(m_ipv4 == pAddr->m_ipv4 && m_iPort == pAddr->m_iPort)
		return TRUE;
	else
		return FALSE;
}
int InetSocketAddress::InitByHostAndPort(char* pHost,int port)
{
	struct addrinfo hints,*res;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int ret=getaddrinfo(pHost,NULL,&hints,&res);
	if(!ret)
	{
		return FALSE;
	}
	struct addrinfo* ptr = res;
	for(;ptr!=NULL;ptr=ptr->ai_next)
	{
		if(ptr->ai_family == AF_INET)
		{
			m_iPort = port;
			m_ipv4 = ((struct sockaddr_in*)(ptr->ai_addr))->sin_addr.s_addr;
			return TRUE;
		}
	}
	return TRUE;
}
int InetSocketAddress::GetIPV4()
{
	return m_ipv4;
}
