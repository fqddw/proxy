#include "Server.h"
#include "InetSocketAddress.h"
#include "CommonType.h"
#include "errno.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "ClientSide.h"
#include "QueuedNetTask.h"
#include "User.h"
#include "arpa/inet.h"
#include "mysql/mysql.h"
#include "sstream"
#include "BlackListIp.h"
void Server::SetPort(int port){
	m_iPort = port;
}

Server::Server():IOHandler()
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	GetEvent()->SetFD(sock);
	GetEvent()->SetIOHandler(this);
	SetServiceType(SERVICE_TYPE_HTTP_PROXY);
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
		{
			   struct sockaddr_in sai;
			   socklen_t len = sizeof(sai);
			   getpeername(client,(struct sockaddr*)&sai,&len);
			   int peerIp = sai.sin_addr.s_addr;
			//printf("Ip %d\n", htonl(peerIp));

			User* pIpUser = User::GetUserByAssociatedIp(htonl(peerIp));
			if(!pIpUser)
			{
			mysql_thread_init();
			MYSQL conn;
			mysql_init(&conn);
			mysql_real_connect(&conn, "localhost", "root","123456", "ts", 0, NULL, 0);
			ostringstream ipstream;
			ipstream<<htonl(peerIp);
			//string sql = string("REPLACE INTO `ipblacklist` set `ip` = '")+ipstream.str()+string("'");
			string sql = "CALL AddIpToIpBlackList('"+ipstream.str()+"')";
			mysql_query(&conn, sql.c_str());

			mysql_close(&conn);
			mysql_thread_end();

			close(client);
			continue;
			}
			else
			delete pIpUser;
			   /*if(IpBlackList::InBlackList(htonl(peerIp)))
			   {
				   close(client);
				   continue;
			   }*/
		}
		ClientSide* pClientSideHandler = new ClientSide(client);
		//pGlobalList->Append(pClientSideHandler);
		pClientSideHandler->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pClientSideHandler->SetMasterThread(GetMasterThread());
		pClientSideHandler->SetCanWrite(FALSE);
		pClientSideHandler->GetEvent()->AddToEngine(EPOLLIN/*|EPOLLET*/|EPOLLONESHOT);
	}
	return TRUE;
}

int Server::IsServer()
{
	return TRUE;
}
