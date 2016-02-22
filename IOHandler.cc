#include "IOHandler.h"
IOHandler::IOHandler():m_pEvent(new IOEvent),m_pSendProc(new SendProccessor(this)),m_pRecvProc(new ReceiveProccessor(this))
{
}
IOEvent* IOHandler::GetEvent()
{
	return m_pEvent;
}

int IOHandler::Run()
{
	Proccess();
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
//			printf("show server epollin\n");
		GetMasterThread()->InsertTask(m_pRecvProc);
	}
	if(events & EPOLLOUT)
	{
			printf("show server epollout\n");
		GetMasterThread()->InsertTask(m_pSendProc);
	}
	if(events & EPOLLERR)
	{
			printf("show server epollerr\n");
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

