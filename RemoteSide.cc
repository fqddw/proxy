#include "RemoteSide.h"
#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
int RemoteSide::Proccess()
{
	return TRUE;
}

int RemoteSide::Writeable()
{
	return TRUE;
}

int RemoteSide::WriteData()
{
	return TRUE;
}

int RemoteSide::SetStatusBlocking()
{
	m_iState = STATUS_BLOCKING; 
	return TRUE;
}

InetSocketAddress* RemoteSide::GetAddr()
{
	return m_pAddr;
}
RemoteSide::RemoteSide()
{
}
RemoteSide::RemoteSide(InetSocketAddress*)
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	m_iSocket = sockfd;
	int cflags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL, cflags|O_NONBLOCK);

}
int RemoteSide::Connect()
{
	printf("%d\n",m_pAddr->GetPort());
	struct sockaddr sa = m_pAddr->ToSockAddr();
	int ret = connect(m_iState,&sa,sizeof(sa));
	return ret;
}
