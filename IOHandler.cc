#include "IOHandler.h"
IOHandler::IOHandler():m_pEvent(new IOEvent),m_pSendProc(new SendProccessor(this)),m_pRecvProc(new ReceiveProccessor(this)),m_bCanRead(TRUE)
{
}
IOEvent* IOHandler::GetEvent()
{
	return m_pEvent;
}

int IOHandler::Run()
{
	return TRUE;
}

int IOHandler::SetMasterThread(MasterThread* pMasterThread)
{
	m_pMasterThread = pMasterThread;
	return TRUE;
}

MasterThread* IOHandler::GetMasterThread()
{
	return m_pMasterThread;
}
IOHandler::~IOHandler()
{
	delete m_pEvent;
}
#include "stdio.h"
int IOHandler::Dispatch(int events)
{
	if(events & EPOLLIN)
	{
		if(m_bCanRead)
		{
			GetMasterThread()->InsertTask(m_pRecvProc);
		}
	}
	if(events & EPOLLOUT)
	{
		printf("FD %d\n",GetEvent()->GetFD());
		GetMasterThread()->InsertTask(m_pSendProc);
	}
	if(events & EPOLLERR)
	{
			printf("EPOLLERR\n");
	}
	return TRUE;
}

int IOHandler::ProccessSend()
{
	return TRUE;
}
int IOHandler::Proccess()
{
	return TRUE;
}

int IOHandler::ProccessConnectionReset()
{
	return TRUE;
}
int IOHandler::ProccessReceive(Stream* pStream)
{
	return TRUE;
}

int IOHandler::IsServer()
{
		return FALSE;
}

