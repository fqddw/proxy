#include "ClientSide.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
#include "MemList.h"
#include "netdb.h"
#include "InetSocketAddress.h"
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
				}
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
		int ret = pRemoteSide->Connect();
	}

	g_pGlobalRemoteSidePool->Unlock();
	return pRemoteSide;
}

