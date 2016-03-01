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
ClientSide::ClientSide():IOHandler(),m_pStream(new Stream())
{
	GetEvent()->SetIOHandler(this);
	m_iState = HEADER_NOTFOUND;
	m_iTransState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
}
ClientSide::ClientSide(int sockfd):IOHandler(),m_pStream(new Stream())
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
						Stream* pHeaderStream = m_pHttpRequest->GetHeader()->ToHeader(); 
						pRemoteSide->GetSendStream()->Append(pHeaderStream->GetData(),pHeaderStream->GetLength());
						int hasBody = m_pHttpRequest->HasBody();
						if(!hasBody)
								m_iState == HEADER_NOTFOUND;
						else
						{
								m_pHttpRequest->LoadBody();
								Stream* pBodyStream = m_pHttpRequest->GetBody()->ToStream(pStream);
								pRemoteSide->GetSendStream()->Append(pStream->GetData(),pBodyStream->GetLength());
						}
				}
		}
		else
		{
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
		pRemoteSide->GetEvent()->AddToEngine(EPOLLIN|EPOLLOUT|EPOLLERR|EPOLLET|EPOLLRDHUP);
		pRemoteSide->SetMasterThread(GetMasterThread());
		int ret = pRemoteSide->Connect();
	}

	g_pGlobalRemoteSidePool->Unlock();
	return pRemoteSide;
}

Stream* ClientSide::GetSendStream(){
	return m_pSendStream;
}

