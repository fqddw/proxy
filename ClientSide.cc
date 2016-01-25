#include "ClientSide.h"
#include "ClientSideTask.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
#include "MemList.h"
extern MemList* pGlobalList;
ClientSide::ClientSide():IOHandler(),m_pStream(new Stream())
{
	m_iIndex = 0;
	GetEvent()->SetIOHandler(this);
	m_iState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
}
ClientSide::ClientSide(int sockfd):IOHandler(),m_pStream(new Stream())
{
	m_iIndex = 0;
	m_iState = CLIENT_STATE_IDLE;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
}

int ClientSide::Proccess()
{
	for(;;)
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
				GetEvent()->RemoveFromEngine();
				if(pGlobalList->Delete(this))
				{
					m_iState = CLIENT_STATE_IDLE;
					delete this;
				}

				close(sockfd);
				return FALSE;
			}
		}
		if(n == 0)
		{
			int sockfd = GetEvent()->GetFD();
			GetEvent()->RemoveFromEngine();
			if(pGlobalList->Delete(this))
			{
				delete this;
			}
			close(sockfd);
			return FALSE;
		}
		m_pStream->Append(buffer,n);

		char* pContent = m_pStream->GetData();
		int length = m_pStream->GetLength();
		char pHeader[1024] = {0};
		const char* tmp1 = "HTTP/1.1 200 OK\r\nServer: Transit-Server 0.0.1\r\nRequest-Tracking: true\r\nContent-Length: ";
		const char* tmp2 = "\r\n\r\n";
		char contentlength[128] = {'\0'}; 
		sprintf(contentlength,"%d",length);
		send(GetEvent()->GetFD(),tmp1,strlen(tmp1),0);
		send(GetEvent()->GetFD(),contentlength,strlen(contentlength),0);
		send(GetEvent()->GetFD(),tmp2,strlen(tmp2),0);
		send(GetEvent()->GetFD(),pContent,length,0);
	}
	m_iState = CLIENT_STATE_IDLE;
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
		m_iState = CLIENT_STATE_RUNNING;
		ClientSideTask* pTask = new ClientSideTask();
		pGlobalList->Append(pTask);
		pTask->SetClientSide(this);
		GetMasterThread()->InsertTask(pTask);
	}
	return TRUE;
};
