#include "AdminServer.h"
#include "InetSocketAddress.h"
#include "CommonType.h"
#include "errno.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "ServerConfig.h"
#include "AdminClient.h"


int AdminServerStartTask::SetNetEngine(NetEngine* pEngine)
{
	m_pEngine = pEngine;
	return TRUE;
}
int AdminServerStartTask::SetMasterThread(MasterThread* pMasterThread)
{
	m_pMasterThread = pMasterThread;
	return TRUE;
}
int AdminServerStartTask::Run()
{
	ServerConfigDefault* pConfig = new ServerConfigDefault();
	AdminServer* pServer = new AdminServer();
	//pGlobalList->Append(pServer);
	pServer->GetEvent()->SetNetEngine(m_pEngine);
	pServer->SetPort(pConfig->GetAdminPort());
	pServer->SetMasterThread(m_pMasterThread);
	pServer->Create();
	pServer->SetCanWrite(FALSE);
	pServer->GetEvent()->AddToEngine(EPOLLET|EPOLLOUT|EPOLLIN|EPOLLERR);
	return TRUE;
}

void AdminServer::SetPort(int port){
	m_iPort = port;
}

AdminServer::AdminServer():IOHandler()
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	GetEvent()->SetFD(sock);
	GetEvent()->SetIOHandler(this);
	SetServiceType(SERVICE_TYPE_ADMIN);
}

int AdminServer::Create()
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
int AdminServer::ProccessReceive(Stream* pStream)
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
						printf("Server Loop Monitor %d\n", errno);
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
		AdminClient* pClientSideHandler = new AdminClient(client);
		//pGlobalList->Append(pClientSideHandler);
		pClientSideHandler->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pClientSideHandler->SetMasterThread(GetMasterThread());
		pClientSideHandler->SetCanWrite(FALSE);
		pClientSideHandler->GetEvent()->AddToEngine(EPOLLIN);
	}
	return TRUE;
}

int AdminServer::IsServer()
{
	return TRUE;
}