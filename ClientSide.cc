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
	if(!pStream)return 0;
		m_pStream->Append(pStream->GetData(),pStream->GetLength());

		if(m_iState == HEADER_NOTFOUND)
		{
			if(m_pHttpRequest->IsHeaderEnd())
			{
				m_pHttpRequest->LoadHttpHeader();
				m_iState = HEADER_FOUND;
				InetSocketAddress* pAddr = NetUtils::GetHostByName(m_pHttpRequest->GetHeader()->GetUrl()->GetHost(),m_pHttpRequest->GetHeader()->GetUrl()->GetPort());

				RemoteSide* pRemoteSide = GetRemoteSide(pAddr);
				m_pRemoteSide = pRemoteSide;
				Stream* pSendStream = m_pHttpRequest->GetHeader()->ToHeader();
				pRemoteSide->GetSendStream()->Append(pSendStream->GetData(),pSendStream->GetLength());
				delete pSendStream;
				m_pStream->Sub(m_pStream->GetLength());
				m_iState = HEADER_NOTFOUND;
				return 0;
				Stream* pHeaderStream = m_pHttpRequest->GetHeader()->ToHeader(); 
				pRemoteSide->GetSendStream()->Append(pHeaderStream->GetData(),pHeaderStream->GetLength());
				int hasBody = m_pHttpRequest->HasBody();
				if(!hasBody)
					m_iState = HEADER_NOTFOUND;
				else
				{
					m_pHttpRequest->LoadBody();
					Stream* pBodyStream = m_pHttpRequest->GetBody()->ToStream(pStream);
					pRemoteSide->GetSendStream()->Append(pStream->GetData(),pBodyStream->GetLength());
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return TRUE;
				if(m_pHttpRequest->GetBody()->IsEnd())
						m_iState = HEADER_NOTFOUND;
				m_pRemoteSide->GetSendStream()->Append(pStream->GetData(),pStream->GetLength());
				m_pRemoteSide->ProccessSend();
		}

		return FALSE;
}

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;

RemoteSide* ClientSide::GetRemoteSide(InetSocketAddress* pAddr)
{
	g_pGlobalRemoteSidePool->Lock();
	RemoteSide* pRemoteSide=NULL;
	MemNode<RemoteSide*>* pSocketPool = g_pGlobalRemoteSidePool->GetHead();
	for(;pSocketPool!=NULL;pSocketPool = pSocketPool->GetNext())
	{
		RemoteSide* pSide = pSocketPool->GetData();
		if(pSide->GetAddr()->Equal(pAddr))
		{
			pSide->SetStatusBlocking();
			g_pGlobalRemoteSidePool->Unlock();
			pRemoteSide = pSide;
			break;
		}
	}

	if(!pRemoteSide)
	{
		pRemoteSide = new RemoteSide(pAddr);
		pRemoteSide->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pRemoteSide->SetMasterThread(GetMasterThread());
		pRemoteSide->GetEvent()->AddToEngine(EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLET|EPOLLRDHUP);
		pRemoteSide->SetClientSide(this);
		//g_pGlobalRemoteSidePool->Append(pRemoteSide);
		int ret = pRemoteSide->Connect();
	}

	g_pGlobalRemoteSidePool->Unlock();
	return pRemoteSide;
}

Stream* ClientSide::GetSendStream(){
	return m_pSendStream;
}


int ClientSide::ProccessSend()
{
		int totalSend = 0;
		int flag = TRUE;
		while(flag)
		{
			if(m_pSendStream->GetLength()==0)
				return FALSE;
				int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
				if(nSent == -1)
				{
						flag = FALSE;
				}
				else
				{
						totalSend += nSent;
						m_pSendStream->Sub(nSent);
						if(m_pSendStream->GetLength() == 0)
						{
							flag = FALSE;
						}
				}
		}
	return TRUE;

}
