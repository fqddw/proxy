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
int IOHandler::Dispatch(int events)
{
	if(events & EPOLLIN)
	{
		GetMasterThread()->InsertTask(m_pRecvProc);
	}
	if(events & EPOLLOUT)
	{
		GetMasterThread()->InsertTask(m_pSendProc);
	}
	if(events & EPOLLERR)
	{
	}
	return TRUE;
}

int IOHandler::Proccess()
{
	return TRUE;
}
