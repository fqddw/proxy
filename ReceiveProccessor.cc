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
#include "NetEngineTask.h"
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
			m_pIOHandler->ReleaseRecvRefCount();
			m_pIOHandler->Release();
		}
		NetEngineTask::getInstance()->GetNetEngine()->Lock();
		NetEngineTask::getInstance()->GetNetEngine()->ReduceTaskCount();
		int iTaskCount = NetEngineTask::getInstance()->GetNetEngine()->GetTaskCount();
		if(iTaskCount == 0)
		{
			NetEngineTask::getInstance()->IncCount();

			NetEngineTask::getInstance()->GetNetEngine()->GetMasterThread()->InsertTask(NetEngineTask::getInstance());
		}
		NetEngineTask::getInstance()->GetNetEngine()->Unlock();

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
		char buffer[4*1024] = {'\0'};
		int n = recv(m_pIOHandler->GetEvent()->GetFD(),buffer,4*1024,0);
		if(n < 0)
		{
			if(errno == EAGAIN)
			{
				printf("Recv EAGAIN\n");
				break;
			}
			else
			{
							if(*ppStream)
								delete *ppStream;
							//m_pIOHandler->SetClosed(TRUE);
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
		break;
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
