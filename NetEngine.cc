#include "NetEngine.h"
#define MAX_WAIT 1024
#define INFINITE -1
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
extern MemList<void*>* pGlobalList;
int NetEngine::Loop()
{
	EPOLLEVENT ees[MAX_WAIT] = {0};
	while(1)
	{
		int nfds = epoll_wait(m_iFD, ees,MAX_WAIT, INFINITE);
		int iterator = 0;
		for(;iterator < nfds; iterator++)
		{
			IOHandler* pHandler = (IOHandler*)((ees+iterator)->data.ptr);
			if(pGlobalList->Find(pHandler))
			{
				pHandler->Dispatch();
			}
			else
				printf("Invalid Handler\n");
		}
	}
	return 0;
}
int NetEngine::RemoveFileDescriptor(IOHandler* pHandler)
{
	EPOLLEVENT ee = {0};
	ee = pHandler->GetEvent()->ToEpollEvent(EPOLLIN|EPOLLOUT|EPOLLERR);
	int ret = epoll_ctl(m_iFD,EPOLL_CTL_DEL,pHandler->GetEvent()->GetFD(),&ee);
	return ret;
}

