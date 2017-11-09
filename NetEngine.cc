#include "NetEngine.h"
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
