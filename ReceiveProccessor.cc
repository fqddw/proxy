#include "ReceiveProccessor.h"
#include "IOHandler.h"
#include "errno.h"
#include "Stream.h"
#include "sys/socket.h"
#include "CommonType.h"
#include "unistd.h"
#include "MemList.h"
#include "stdio.h"
#include "RemoteSide.h"
extern MemList<void*>* pGlobalList;
extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;
ReceiveProccessor::ReceiveProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler)
{
}

int ReceiveProccessor::Run()
{
		Stream* pStream = NULL;
		if(m_pIOHandler->IsServer()){
			m_pIOHandler->ProccessReceive(pStream);
		}else{
			m_pIOHandler->GetEvent()->CancelInReady();
			GetDataStream(&pStream);
			m_pIOHandler->ProccessReceive(pStream);
		}
		return 0;
}

int ReceiveProccessor::GetDataStream(Stream** pStream)
{
	*pStream = NULL;
	for(;;)
	{
		char buffer[256*1024] = {'\0'};
		int n = recv(m_pIOHandler->GetEvent()->GetFD(),buffer,256*1024,0);
		if(n < 0)
		{
			if(errno == EAGAIN)
			{
				return TRUE;
			}
			else if(errno == EINTR)
			{
				printf("EINR\n");
				continue;
			}
			else
			{
				printf("ERROR\n");
				return FALSE;
			}
		}
		if(n == 0)
		{
			int sockfd = m_pIOHandler->GetEvent()->GetFD();
			m_pIOHandler->GetEvent()->RemoveFromEngine();
			if(pGlobalList->Delete(m_pIOHandler))
			{
				//delete m_pIOHandler;
			}
			/*if(g_pGlobalRemoteSidePool->Delete(reinterpret_cast<RemoteSide*>(m_pIOHandler)))
			{
			}*/
			close(sockfd);
			return FALSE;
		}
		if(*pStream == NULL)
			*pStream = new Stream();
		(*pStream)->Append(buffer,n);
	}
	return TRUE;
}

ReceiveProccessor::~ReceiveProccessor()
{
}
