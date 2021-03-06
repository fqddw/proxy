#ifndef __INET_SOCKET_ADDRESS_H__
#define __INET_SOCKET_ADDRESS_H__
#include "sys/socket.h"
#include "netinet/in.h"
#include "netdb.h"
class InetSocketAddress
{
	public:
		InetSocketAddress();
		InetSocketAddress(int,in_addr_t);
		struct sockaddr ToSockAddr();
		struct sockaddr_in ToSockAddrIn();
		int ToIpV4Short();
		int GetIPV4();
		int GetPort();
		void SetPort(int);
		int Size();
		int Equal(InetSocketAddress*);
		int InitByHostAndPort(char*,int);
	private:
		int m_iPort;
		unsigned int m_ipv4;
};
#endif
