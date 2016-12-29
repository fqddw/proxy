#include "Server.h"
#include "InetSocketAddress.h"
#include "CommonType.h"
#include "errno.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "ClientSide.h"
void Server::SetPort(int port){
	m_iPort = port;
}

Server::Server():IOHandler()
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	GetEvent()->SetFD(sock);
	GetEvent()->SetIOHandler(this);
}

int Server::Create()
{
	int sockfd = GetEvent()->GetFD();
	int cflags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL, cflags|O_NONBLOCK);

	InetSocketAddress isa(m_iPort,INADDR_ANY);
	struct sockaddr sa = isa.ToSockAddr();
	int flag = TRUE;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&flag,sizeof(int));
	bind(sockfd,&sa,isa.Size());
	listen(sockfd,1024);
	return true;
}

#include "MemList.h"
extern MemList<void*>* pGlobalList;
int Server::ProccessReceive(Stream* pStream)
{
	struct sockaddr sa = {0};
	socklen_t len = sizeof(sa);
	int flag = TRUE;
	while(flag)
	{
		int client = accept(GetEvent()->GetFD(),&sa,&len);
		if(client == -1 && errno == EAGAIN)
		{
			return TRUE;
		}
		else if(client == -1)
		{
			GetEvent()->RemoveFromEngine();
			close(GetEvent()->GetFD());
			if(pGlobalList->Delete(this))
			{
				delete this;
			}
			return FALSE;
		}
		else
		{
		}
		int cflags = fcntl(client,F_GETFL,0);
		fcntl(client,F_SETFL, cflags|O_NONBLOCK);
		ClientSide* pClientSideHandler = new ClientSide(client);
		//pGlobalList->Append(pClientSideHandler);
		pClientSideHandler->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pClientSideHandler->SetMasterThread(GetMasterThread());
		pClientSideHandler->SetCanWrite(FALSE);
		pClientSideHandler->GetEvent()->AddToEngine(EPOLLOUT|EPOLLIN|EPOLLERR|EPOLLET|EPOLLRDHUP);
	}
	return TRUE;
}

int Server::IsServer()
{
	return TRUE;
}
