#include "ClientSide.h"
#include "ClientSideTask.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
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
		if(n == -1)
		{
			if(errno == EAGAIN)
			{
				return TRUE;
			}
			else
			{
				GetEvent()->RemoveFromEngine();
				return FALSE;
			}
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
		pTask->SetClientSide(this);
		GetMasterThread()->InsertTask(pTask);
	}
	return TRUE;
};
