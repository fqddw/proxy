#include "ReceiveProccessor.h"
#include "IOHandler.h"
#include "errno.h"
#include "Stream.h"
#include "sys/socket.h"
#include "CommonType.h"
#include "unistd.h"
#include "MemList.h"
#include "stdio.h"
extern MemList<void*>* pGlobalList;
ReceiveProccessor::ReceiveProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler)
{
}

int ReceiveProccessor::Run()
{
		Stream* pStream = NULL;
		if(m_pIOHandler->IsServer()){
			printf("server in here\n");
				m_pIOHandler->ProccessReceive(pStream);
		}else{
				GetDataStream(&pStream);
				if(pStream)
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
				continue;
			}
			else
			{
				return FALSE;
			}
		}
		if(n == 0)
		{
			int sockfd = m_pIOHandler->GetEvent()->GetFD();
			m_pIOHandler->GetEvent()->RemoveFromEngine();
			if(pGlobalList->Delete(this))
			{
				delete this;
			}
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
