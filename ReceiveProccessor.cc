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
ReceiveProccessor::ReceiveProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler),Task()
{
}

int ReceiveProccessor::Run()
{
		Stream* pStream = NULL;
		if(m_pIOHandler->IsServer()){
			m_pIOHandler->ProccessReceive(pStream);
		}else{
			m_pIOHandler->GetEvent()->CancelInReady();
			int ret = GetDataStream(&pStream);
		}
		return 0;
}
#include "RemoteSide.h"
extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;
int ReceiveProccessor::GetDataStream(Stream** ppStream)
{
	*ppStream = NULL;
	int total = 0;
	m_pIOHandler->SetCanRead(FALSE);
	for(;;)
	{
		char buffer[256*1024] = {'\0'};
		int n = recv(m_pIOHandler->GetEvent()->GetFD(),buffer,256*1024,0);
		if(n < 0)
		{
			if(errno == EAGAIN)
			{
				break;
			}
			else if(errno == EINTR)
			{
				printf("EINR\n");return 0;
				//continue;
			}
			else
			{
						if(m_pIOHandler->GetSide() == CLIENT_SIDE)
							printf("sig pipe client %d\n", m_pIOHandler->GetSendStream()->GetLength());
						if(m_pIOHandler->GetSide() == REMOTE_SIDE)
							printf("sig pipe remote %d\n", m_pIOHandler->GetSendStream()->GetLength());

						printf("ClearHttpEnd %s %d\n", __FILE__, __LINE__);
						m_pIOHandler->ClearHttpEnd();
				int sockfd = m_pIOHandler->GetEvent()->GetFD();
				m_pIOHandler->GetEvent()->RemoveFromEngine();
				if(pGlobalList->Delete(m_pIOHandler))
				{
				}
				if(m_pIOHandler->GetSide() == REMOTE_SIDE)
				{
					if(g_pGlobalRemoteSidePool->Delete(reinterpret_cast<RemoteSide*>(m_pIOHandler)))
					{
					}
				}
				close(sockfd);

				return FALSE;
				/*printf("ERROR\n");
				int sockfd = m_pIOHandler->GetEvent()->GetFD();
				m_pIOHandler->GetEvent()->RemoveFromEngine();
				g_pGlobalRemoteSidePool->Delete((RemoteSide*)m_pIOHandler);

				return FALSE;*/
			}
		}
		if(n == 0)
		{
						/*
						if(m_pIOHandler->GetSide() == CLIENT_SIDE)
										printf("client close %d\n", m_pIOHandler->GetSendStream()->GetLength());

						if(m_pIOHandler->GetSide() == REMOTE_SIDE)
										printf("remote close %d\n", m_pIOHandler->GetSendStream()->GetLength());
										*/
						printf("ClearHttpEnd %s %d %d %d\n", __FILE__, __LINE__, m_pIOHandler->GetSide(), errno);
			//m_pIOHandler->ClearHttpEnd();
			int sockfd = m_pIOHandler->GetEvent()->GetFD();
			m_pIOHandler->GetEvent()->RemoveFromEngine();
			g_pGlobalRemoteSidePool->Delete((RemoteSide*)m_pIOHandler);
			if(pGlobalList->Delete(m_pIOHandler))
			{
				//delete m_pIOHandler;
			}
			//m_pIOHandler->ProccessConnectionReset();
			if(m_pIOHandler->GetSide() == REMOTE_SIDE)
			{
				if(g_pGlobalRemoteSidePool->Delete(reinterpret_cast<RemoteSide*>(m_pIOHandler)))
				{
				}
			}

			close(sockfd);
			break;
		}
		total += n;
		if(*ppStream == NULL)
			*ppStream = new Stream();
		(*ppStream)->Append(buffer,n);
		//break;
	}
		//m_pIOHandler->SetCanRead(TRUE);

	if(total > 0)
	{
		m_pIOHandler->ProccessReceive(*ppStream);
	}
	else
	{
					m_pIOHandler->SetCanRead(TRUE);
	}

	return TRUE;
}

ReceiveProccessor::~ReceiveProccessor()
{
}
