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
	m_iIndex = 0;
	GetEvent()->SetIOHandler(this);
	m_iState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
}
ClientSide::ClientSide(int sockfd):IOHandler()
{
	m_iIndex = 0;
	m_iState = CLIENT_STATE_IDLE;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
}

int ClientSide::Proccess()
{
	m_iState = CLIENT_STATE_RUNNING;
	while(1)
	{
		char buffer[1024] = {'\0'};
		int n = recv(GetEvent()->GetFD(),buffer,1024,0);
		if(n < 0)
		{
			if(errno == EAGAIN)
			{
				m_iState = CLIENT_STATE_IDLE;
				return TRUE;
			}
			else if(errno == EINTR)
			{
				continue;
			}
			else
			{
				int sockfd = GetEvent()->GetFD();
				//printf("CONNECT ERROR %d\n",sockfd);
				GetEvent()->RemoveFromEngine();
				if(pGlobalList->Find(this))
				{
					m_iState = CLIENT_STATE_IDLE;
					pGlobalList->Delete(this);
					delete this;
				}

				close(GetEvent()->GetFD());
				return FALSE;
			}
		}
		if(n == 0)
		{
			if(!pGlobalList->Find(this))
			{
				printf("ERROR IOHandler Memory\n");
			}
			printf("mIndex %d %d\n",GetEvent()->GetFD(),m_iIndex);
			m_iIndex++;

			int sockfd = GetEvent()->GetFD();
			GetEvent()->RemoveFromEngine();
			if(pGlobalList->Find(this))
			{
				printf("SHOULDSHOWDELETE %d\n",this);
				pGlobalList->Delete(this);
				delete this;
			}
			close(sockfd);
			m_iState = CLIENT_STATE_IDLE;
			return FALSE;
		}

		char* pContent = "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n<title>vpn1g</title>";
		send(GetEvent()->GetFD(),pContent,strlen(pContent),0);
	}
	m_iState = CLIENT_STATE_IDLE;
	return TRUE;
}

int ClientSide::Run()
{
	if(m_iState != CLIENT_STATE_IDLE)
	{
		printf("BLOCK\n");
		return FALSE;
	}
	else
	{
		ClientSideTask* pTask = new ClientSideTask();
		pGlobalList->Append(pTask);
		pTask->SetClientSide(this);
		GetMasterThread()->InsertTask(pTask);
	}
	return TRUE;
};
