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
			m_pIOHandler->Release();
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
			else
			{
							if(*ppStream)
											delete *ppStream;
							m_pIOHandler->ProccessConnectionReset();
							return FALSE;
			}
		}
		if(n == 0)
		{
						m_pIOHandler->SetClosed(TRUE);
			break;
		}
		//printf("%s", buffer);
		total += n;
		if(*ppStream == NULL)
			*ppStream = new Stream();
		(*ppStream)->Append(buffer,n);
		//break;
	}
		//m_pIOHandler->SetCanRead(TRUE);

	if(total >= 0)
	{
		m_pIOHandler->ProccessReceive(*ppStream);
	}
	return TRUE;
}

ReceiveProccessor::~ReceiveProccessor()
{
}
