#include "ClientSide.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
#include "MemList.h"
#include "netdb.h"
#include "InetSocketAddress.h"
#include "NetUtils.h"
#include "arpa/inet.h"
extern MemList<void*>* pGlobalList;
#define HEADER_NOTFOUND 0
#define HEADER_FOUND 1
ClientSide::ClientSide():IOHandler(),m_pStream(new Stream()),m_pSendStream(new Stream())
{
	GetEvent()->SetIOHandler(this);
	m_iState = HEADER_NOTFOUND;
	m_iTransState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
}
ClientSide::ClientSide(int sockfd):IOHandler(),m_pStream(new Stream()),m_pSendStream(new Stream())
{
	m_iTransState = CLIENT_STATE_IDLE;
	m_iState = HEADER_NOTFOUND;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
	m_pHttpRequest = new HttpRequest(m_pStream);
}

int ClientSide::Proccess()
{
		return FALSE;
}
int ClientSide::ProccessReceive(Stream* pStream)
{
	if(!pStream)
	{
		GetEvent()->CancelInReady();
		SetCanRead(TRUE);
		return 0;
	}
		m_pStream->Append(pStream->GetData(),pStream->GetLength());


		if(m_iState == HEADER_NOTFOUND && m_iTransState != CLIENT_STATE_WAITING)
		{
			if(m_iTransState != CLIENT_STATE_RUNNING)
				m_iTransState = CLIENT_STATE_RUNNING;
			if(m_pHttpRequest->IsHeaderEnd())
			{
				m_pHttpRequest->LoadHttpHeader();
				m_pHttpRequest->GetAuthStatus();
				m_iState = HEADER_FOUND;
				InetSocketAddress* pAddr = NULL;
				pAddr = NetUtils::GetHostByName(m_pHttpRequest->GetHeader()->GetUrl()->GetHost(),m_pHttpRequest->GetHeader()->GetUrl()->GetPort());
				if(!pAddr)
				{
					char* pText = (char*)"HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
					send(GetEvent()->GetFD(),pText,strlen(pText),0);
					return 0;
				}
				RemoteSide* pRemoteSide = GetRemoteSide(pAddr);
				m_pRemoteSide = pRemoteSide;
				Stream* pSendStream = m_pHttpRequest->GetHeader()->ToHeader();
				pRemoteSide->GetSendStream()->Append(pSendStream->GetData(),pSendStream->GetLength());
				delete pSendStream;
				SetCanWrite(TRUE);
				//GetEvent()->ModEvent(EPOLLOUT|EPOLLET);

				int hasBody = m_pHttpRequest->HasBody();
				if(!hasBody)
				{
					delete m_pHttpRequest;
					m_pHttpRequest = new HttpRequest(m_pStream);
					m_iState = HEADER_NOTFOUND;
					m_iTransState = CLIENT_STATE_WAITING;
				}
				else
				{
					m_pHttpRequest->LoadBody();
					Stream* pBodyStream = m_pStream->GetPartStream(m_pHttpRequest->GetHeader()->GetRawLength(),m_pStream->GetLength());
					if(m_pHttpRequest->GetBody()->IsEnd(pBodyStream))
					{
						m_iState = HEADER_NOTFOUND;
						m_iTransState = CLIENT_STATE_WAITING;
					}

					pRemoteSide->GetSendStream()->Append(pBodyStream->GetData(),pBodyStream->GetLength());
				}
				if(pRemoteSide->IsConnected())
				{
					pRemoteSide->SetCanWrite(TRUE);
					pRemoteSide->ProccessSend();
				}
				m_pStream->Sub(m_pStream->GetLength());
			}
			else
			{
				return FALSE;
			}
		}
		else if(m_iState == HEADER_FOUND && m_iTransState == CLIENT_STATE_RUNNING)
		{
				if(m_pHttpRequest->GetBody()->IsEnd(pStream))
				{
						m_iState = HEADER_NOTFOUND;
						m_iTransState = CLIENT_STATE_WAITING;
				}
				m_pRemoteSide->GetSendStream()->Append(pStream->GetData(),pStream->GetLength());
				m_pRemoteSide->ProccessSend();
				m_pStream->Sub(m_pStream->GetLength());
		}
		else if(m_iTransState == CLIENT_STATE_WAITING)
		{
			close(m_pRemoteSide->GetEvent()->GetFD());
			m_pRemoteSide->SetClientSide(NULL);
		}

		return FALSE;
}

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;

RemoteSide* ClientSide::GetRemoteSide(InetSocketAddress* pAddr)
{
	//g_pGlobalRemoteSidePool->Lock();
	RemoteSide* pRemoteSide=NULL;
	MemNode<RemoteSide*>* pSocketPool = g_pGlobalRemoteSidePool->GetHead();
	for(;pSocketPool!=NULL;pSocketPool = pSocketPool->GetNext())
	{
		RemoteSide* pSide = pSocketPool->GetData();
		if(pSide->GetAddr()->Equal(pAddr) && pSide->IsIdle())
		{
			pSide->SetStatusBlocking();
			//g_pGlobalRemoteSidePool->Unlock();
			pRemoteSide = pSide;
			pRemoteSide->SetClientSide(this);
			break;
		}
	}

	if(!pRemoteSide)
	{
		pRemoteSide = new RemoteSide(pAddr);
		pRemoteSide->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pRemoteSide->SetMasterThread(GetMasterThread());
		pRemoteSide->SetClientSide(this);
		pRemoteSide->GetEvent()->AddToEngine(EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLET|EPOLLRDHUP);
		g_pGlobalRemoteSidePool->Append(pRemoteSide);
	}
	//g_pGlobalRemoteSidePool->Unlock();
	return pRemoteSide;
}

Stream* ClientSide::GetSendStream(){
	return m_pSendStream;
}


int ClientSide::ProccessSend()
{
	//printf("Send Pending Length: %d\n",m_pSendStream->GetLength());
	if(m_pSendStream->GetLength()<=0)
	{
		SetCanWrite(FALSE);
		return FALSE;
	}
		int totalSend = 0;
		int flag = TRUE;
		while(flag)
		{
			LockSendBuffer();
			int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
			if(nSent == -1)
			{
				flag = FALSE;
				if(errno == EAGAIN)
				{
					if(GetEvent()->IsOutReady())
					{
						GetEvent()->CancelOutReady();
						GetMasterThread()->InsertTask(GetSendTask());
						UnlockSendBuffer();
						return TRUE;
					}
				}
				else
				{
					m_pRemoteSide->GetEvent()->RemoveFromEngine();
					close(m_pRemoteSide->GetEvent()->GetFD());
					int sockfd = GetEvent()->GetFD();
					GetEvent()->RemoveFromEngine();
					if(pGlobalList->Delete(this))
					{
						//delete m_pIOHandler;
					}
					close(sockfd);
				}
			}
			else
			{
				totalSend += nSent;
				m_pSendStream->Sub(nSent);
				if(m_pSendStream->GetLength() == 0)
				{
					if(m_pRemoteSide->GetResponse()->GetBody())
					if(m_pRemoteSide->GetResponse()->GetBody()->IsEnd())
					{
						m_iTransState = CLIENT_STATE_IDLE;
						m_pRemoteSide->SetStatusIdle();
						SetCanWrite(FALSE);
						//GetEvent()->ModEvent(EPOLLIN|EPOLLERR|EPOLLET|EPOLLRDHUP);
					}
					//SetCanWrite(TRUE);
					if(m_pRemoteSide->GetEvent()->IsInReady())
					{
						m_pRemoteSide->GetEvent()->CancelInReady();
						UnlockSendBuffer();
						GetMasterThread()->InsertTask(m_pRemoteSide->GetRecvTask());
						return TRUE;
					}
					else
					{
						m_pRemoteSide->SetCanRead(TRUE);
					}
					flag = FALSE;
				}
			}
		}
	return FALSE;

}
void ClientSide::SetTransIdleState()
{
	m_iTransState = CLIENT_STATE_IDLE;
}
int ClientSide::ProccessConnectionReset()
{
	if(m_pRemoteSide)
	{
		m_pRemoteSide->SetClientSide(NULL);
	}
}
