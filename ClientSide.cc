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
ClientSide::ClientSide():IOHandler(),m_pStream(new Stream())
{
	GetEvent()->SetIOHandler(this);
	m_iState = CLIENT_STATE_IDLE;
	m_iTransState = HEADER_NOTFOUND;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
}
ClientSide::ClientSide(int sockfd):IOHandler(),m_pStream(new Stream())
{
	m_iTransState = HEADER_NOTFOUND;
	m_iState = CLIENT_STATE_IDLE;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
}

int ClientSide::Proccess()
{
		return FALSE;
}
int ClientSide::ProccessReceive(Stream* pStream)
{
		printf("%s",pStream->GetData());
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

