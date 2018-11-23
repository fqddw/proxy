#include "NetEngine.h"
#include "QueuedNetTask.h"
class RemoteSide;
extern MemList<void*>* pGlobalList;
extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;

int NetEngine::Init(){
	m_iFD = epoll_create(m_iSize);
	if(m_iFD)
		return true;
	else
		return false;
}

int NetEngine::AddFileDescriptor(IOHandler* pHandler,int events)
{
	EPOLLEVENT ee = {0};
	ee = pHandler->GetEvent()->ToEpollEvent(events);
	int ret = epoll_ctl(m_iFD,EPOLL_CTL_ADD,pHandler->GetEvent()->GetFD(),&ee);
	return ret;
}

int NetEngine::SetSize(int size)
{
	m_iSize = size;
	return true;
}
#include "MemList.h"
#include "stdio.h"
#include "memory.h"
#include "errno.h"
extern MemList<void*>* pGlobalList;
int NetEngine::Loop()
{
	EPOLLEVENT* ees=m_pEvents;
	while(1)
	{
		m_iNFDS = epoll_wait(m_iFD, ees,MAX_WAIT, INFINITE);
		int iterator = 0;
		for(;iterator < m_iNFDS; iterator++)
		{
			IOHandler* pHandler = (IOHandler*)((ees+iterator)->data.ptr);
			/*if((ees+iterator)->events & EPOLLERR)
			{
				printf("EPOLLERR %d\n",(ees+iterator)->events);
				continue;
			}*/
			if(!pHandler)
			{
				printf("NULL HANDLER\n");
				continue;
			}
			if(1)//pGlobalList->Find(pHandler))
			{
				int events = (ees+iterator)->events;
				if( !(events & EPOLLIN) && !(events & EPOLLOUT))
				{
					continue;
				}
				pHandler->Dispatch((ees+iterator)->events);
			}
			else
				printf("Invalid Handler\n");
			/*if(pHandler->IsRealClosed())
			{
				printf("Ref Zero %d %d\n", (ees+iterator)->events, errno);
				continue;
			}*/

		}
		m_iNFDS = 0;
	}
	return 0;
}
int NetEngine::RemoveFileDescriptor(IOHandler* pHandler)
{
	EPOLLEVENT ee = {0};
	ee = pHandler->GetEvent()->ToEpollEvent(EPOLLERR|EPOLLET);
	ee.data.ptr = NULL;
	/*Lock();
	int i = 0;
	int bFind = FALSE;
	for(;i < m_iNFDS; i++)
	{
		if(!bFind)
		{
			if(((IOHandler*)(m_pEvents+i)->data.ptr)->GetEvent()->GetFD() == pHandler->GetEvent()->GetFD())
			{
				bFind = TRUE;
			}
		}
		else
		{
			memcpy(m_pEvents+i-1, m_pEvents+i, sizeof(EPOLLEVENT));
		}
	}
	if(bFind)
		m_iNFDS--;*/
	int ret = epoll_ctl(m_iFD,EPOLL_CTL_DEL,pHandler->GetEvent()->GetFD(),&ee);
	//Unlock();
	return ret;
}
int NetEngine::ModFileDescriptor(IOHandler* pHandler,int event)
{
	EPOLLEVENT ee = {0};
	ee = pHandler->GetEvent()->ToEpollEvent(event);
	int ret = epoll_ctl(m_iFD,EPOLL_CTL_MOD,pHandler->GetEvent()->GetFD(),&ee);
	if(ret == -1)
		printf("epoll_ctl error %d\n", event);
	return ret;
}

NetEngine::NetEngine():cs_(new CriticalSection()),m_iNFDS(0)
{
}

void NetEngine::Lock()
{
	cs_->Enter();
}

void NetEngine::Unlock()
{
	cs_->Leave();
}

typedef struct _task_detail
{
	Task* pTask;
	int type;
}TaskDetail;
#define TASK_SERVER 1
#define TASK_NULL 2
#define TASK_QUEUED 3
int NetEngine::Run()
{
	EPOLLEVENT* ees=m_pEvents;
	TaskDetail pTaskArray[MAX_WAIT] = {0};
	int iTaskSize = 0;
	m_iNFDS = epoll_wait(m_iFD, ees,MAX_WAIT, INFINITE);
	int iterator = 0;
	for(;iterator < m_iNFDS; iterator++)
	{
		IOHandler* pHandler = (IOHandler*)((ees+iterator)->data.ptr);
		if(pHandler->IsServer())
		{
			pTaskArray[iTaskSize].pTask = pHandler->GetRecvTask();
			pTaskArray[iTaskSize].type = TASK_SERVER;
			iTaskSize++;
			continue;
		}
		QueuedNetTask* pTask = pHandler->GetMainTask();
		if(!pTask)
		{
			pTaskArray[iTaskSize].pTask = pHandler->GetRecvTask();
			pTaskArray[iTaskSize].type = TASK_NULL;
			iTaskSize++;
			g_pGlobalRemoteSidePool->Delete((RemoteSide*)pHandler);
		}
		else
		{
			int flag = FALSE;
			int i = 0;
			for(i = 0; i < iTaskSize; i++)
			{
				if(pTask == pTaskArray[i].pTask)
				{
					flag = TRUE;
					break;
				}
			}
			if(flag == FALSE)
			{
				pTaskArray[iTaskSize].pTask = pTask;
				pTaskArray[iTaskSize].type = TASK_QUEUED;
				iTaskSize = iTaskSize + 1;
			}
			pHandler->Schedule((ees+iterator)->events);
		}
	}

	if(m_iTaskCount < 0)
	{
		printf("count %d\n", m_iTaskCount);
	}
	m_iTaskCount = iTaskSize;

	int taskit = 0;
	for(;taskit<iTaskSize;taskit++)
	{
		if(pTaskArray[taskit].type == TASK_QUEUED)
		{
			QueuedNetTask* pQueuedTask = (QueuedNetTask*)pTaskArray[taskit].pTask;
			pQueuedTask->Lock();
			pQueuedTask->IncCount();
			pQueuedTask->Unlock();
		}
		GetMasterThread()->InsertTask(pTaskArray[taskit].pTask);
	}

	return TRUE;
}

MasterThread* NetEngine::GetMasterThread()
{
	return m_pMasterThread;
}

int NetEngine::SetMasterThread(MasterThread* pMasterThread)
{
	m_pMasterThread = pMasterThread;
	return TRUE;
}

void NetEngine::IncTaskCount()
{
	m_iTaskCount = m_iTaskCount + 1;
}

void NetEngine::ReduceTaskCount()
{
	m_iTaskCount = m_iTaskCount - 1;
}

int NetEngine::GetTaskCount()
{
	return m_iTaskCount;
}
