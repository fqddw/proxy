#include "Server.h"
#include "InetSocketAddress.h"
#include "CommonType.h"

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
	InetSocketAddress isa(m_iPort,INADDR_ANY);
	struct sockaddr sa = isa.ToSockAddr();
	int flag = TRUE;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&flag,sizeof(int));
	bind(sockfd,&sa,isa.Size());
	listen(sockfd,1024);
	return true;
}

int Server::ProccessRequest()
{
	return 0;
}

int Server::Run()
{
	struct sockaddr sa = {0};
	socklen_t len = sizeof(sa);
	int client = accept(GetEvent()->GetFD(),&sa,&len);
	int cflags = fcntl(client,F_GETFL,0);
	fcntl(client,F_SETFL, cflags|O_NONBLOCK);
	ClientSide* pClientSideHandler = new ClientSide(client);
	pClientSideHandler->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
	pClientSideHandler->GetEvent()->AddToEngine();
	pClientSideHandler->SetMasterThread(GetMasterThread());
	return TRUE;
}

