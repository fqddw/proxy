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

		if(m_iState == HEADER_NOTFOUND)
		{
			if(m_pHttpRequest->IsHeaderEnd())
			{
				m_pHttpRequest->LoadHttpHeader();
				m_iState = HEADER_FOUND;
				InetSocketAddress* pAddr = NULL;
				pAddr = NetUtils::GetHostByName(m_pHttpRequest->GetHeader()->GetUrl()->GetHost(),m_pHttpRequest->GetHeader()->GetUrl()->GetPort());
				RemoteSide* pRemoteSide = GetRemoteSide(pAddr);
				m_pRemoteSide = pRemoteSide;
				Stream* pSendStream = m_pHttpRequest->GetHeader()->ToHeader();
				pRemoteSide->GetSendStream()->Append(pSendStream->GetData(),pSendStream->GetLength());
				delete pSendStream;
				m_pStream->Sub(m_pStream->GetLength());
				m_iState = HEADER_NOTFOUND;
				SetCanWrite(TRUE);

				if(pRemoteSide->IsConnected())
				{
					pRemoteSide->SetCanWrite(TRUE);
					pRemoteSide->ProccessSend();
				}
				return 0;
				Stream* pHeaderStream = m_pHttpRequest->GetHeader()->ToHeader(); 
				pRemoteSide->GetSendStream()->Append(pHeaderStream->GetData(),pHeaderStream->GetLength());
				int hasBody = m_pHttpRequest->HasBody();
				if(!hasBody)
					m_iState = HEADER_NOTFOUND;
				else
				{
					/*m_pHttpRequest->LoadBody();
					Stream* pBodyStream = m_pHttpRequest->GetBody()->ToStream(pStream);
					pRemoteSide->GetSendStream()->Append(pStream->GetData(),pBodyStream->GetLength());*/
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
				if(m_pHttpRequest->GetBody()->IsEnd(pStream))
						m_iState = HEADER_NOTFOUND;
				m_pRemoteSide->GetSendStream()->Append(pStream->GetData(),pStream->GetLength());
				m_pRemoteSide->ProccessSend();
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
		pRemoteSide->GetEvent()->AddToEngine(EPOLLOUT|EPOLLERR|EPOLLET|EPOLLRDHUP);
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
	printf("Send Pending Length: %d\n",m_pSendStream->GetLength());
	if(m_pSendStream->GetLength()<=0)
	{
		SetCanWrite(FALSE);
		return FALSE;
	}
		int totalSend = 0;
		int flag = TRUE;
		LockSendBuffer();
		while(flag)
		{
			int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
			if(nSent == -1)
			{
				flag = FALSE;
				if(GetEvent()->IsOutReady())
				{
					GetEvent()->CancelOutReady();
					//GetMasterThread()->InsertTask(GetSendTask());
					return TRUE;
				}
			}
			else
			{
				totalSend += nSent;
				m_pSendStream->Sub(nSent);
				if(m_pSendStream->GetLength() == 0)
				{
					if(m_pRemoteSide->GetResponse()->GetBody()->IsEnd())
					{
						m_pRemoteSide->SetStatusIdle();
						SetCanWrite(FALSE);
					}
					//SetCanWrite(TRUE);
					UnlockSendBuffer();
					if(m_pRemoteSide->GetEvent()->IsInReady())
					{
						m_pRemoteSide->GetEvent()->CancelInReady();
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
		UnlockSendBuffer();
	return FALSE;

}
