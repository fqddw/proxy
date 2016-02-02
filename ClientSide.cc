#include "ClientSide.h"
#include "ClientSideTask.h"
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
ClientSide::ClientSide():DataIOHandler(),m_pStream(new Stream())
{
	GetEvent()->SetIOHandler(this);
	m_iState = CLIENT_STATE_IDLE;
	m_iTransState = HEADER_NOTFOUND;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
}
ClientSide::ClientSide(int sockfd):DataIOHandler(),m_pStream(new Stream())
{
	m_iTransState = HEADER_NOTFOUND;
	m_iState = CLIENT_STATE_IDLE;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
}

int ClientSide::Proccess()
{
	Stream* pStream = NULL;
	GetDataStream(&pStream);
	if(pStream)
	{
		m_pStream->Append(pStream->GetData(),pStream->GetLength());
		if(!m_pHttpRequest)
		{
			m_pHttpRequest = new HttpRequest(m_pStream);
			m_iTransState == HEADER_NOTFOUND;
		}
		if(m_iTransState == HEADER_NOTFOUND)
		{
			if(m_pHttpRequest->IsHeaderEnd())
			{
				int ret = m_pHttpRequest->LoadHttpHeader();
				HttpHeader* pHttpHeader = m_pHttpRequest->GetHeader();
				InetSocketAddress *pAddr = new InetSocketAddress();
				char* pHostName = pHttpHeader->GetRequestLine()->GetUrl()->GetHost();
				int port = pHttpHeader->GetRequestLine()->GetUrl()->GetPort();
				pAddr->InitByHostAndPort(pHostName,port);
				RemoteSide* pRemoteSide = GetRemoteSide(pAddr);
				if(pRemoteSide->Writeable())
				{
					pRemoteSide->WriteData();
				}
			};
		}
		return TRUE;
	}
	else
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

