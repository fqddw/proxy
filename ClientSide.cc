#include "ClientSide.h"
#include "ClientSideTask.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
#include "MemList.h"
extern MemList* pGlobalList;
ClientSide::ClientSide():IOHandler()
{
	GetEvent()->SetIOHandler(this);
	m_iState = CLIENT_STATE_IDLE;
}

ClientSide::ClientSide(int sockfd):IOHandler()
{
	m_iState = CLIENT_STATE_IDLE;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
}

int ClientSide::Proccess()
{
	while(1)
	{
		char buffer[1024] = {'\0'};
		int n = recv(GetEvent()->GetFD(),buffer,1024,0);
		if(n < 0)
		{
			if(errno == EAGAIN)
			{
				printf("EAGAIN\n");
				m_iState = CLIENT_STATE_IDLE;
				return TRUE;
			}
			else if(errno == EINTR)
			{
				printf("EINTR\n");
				continue;
			}
			else
			{
				printf("UNNORMAL CLOSE\n");
				GetEvent()->RemoveFromEngine();
				close(GetEvent()->GetFD());
				if(pGlobalList->Find(this)){
					pGlobalList->Delete(this);
					delete this;
				}
				return FALSE;
			}
		}
		if(n == 0)
		{
			printf("NORMAL CLOSE\n");
			if(errno == EINTR)
			{
				printf("NORMAL CLOSE EINTR\n");
				continue;
			}
			GetEvent()->RemoveFromEngine();
			close(GetEvent()->GetFD());
			if(pGlobalList->Find(this)){
				pGlobalList->Delete(this);
				delete this;
			}
			return FALSE;
		}

		char* pContent = "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n<title>vpn1g</title>";
		send(GetEvent()->GetFD(),pContent,strlen(pContent),0);
	}
	return TRUE;
}

int ClientSide::Run()
{
	if(m_iState != CLIENT_STATE_IDLE)
	{
		return FALSE;
	}
	else
	{
		ClientSideTask* pTask = new ClientSideTask();
		pGlobalList->Append(pTask);
		printf("PRE MEM %d\n",pTask);
		pTask->SetClientSide(this);
		GetMasterThread()->InsertTask(pTask);
	}
	return TRUE;
};
