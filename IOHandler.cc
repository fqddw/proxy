#include "IOHandler.h"
IOHandler::IOHandler():m_pEvent(new IOEvent),Task()
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
	GetMasterThread()->InsertTask(this);
	if(events & EPOLLIN)
	{
	}
	if(events & EPOLLOUT)
	{
	}
	if(events & EPOLLOUT)
	{
	}
	return TRUE;
}

int IOHandler::Proccess()
{
	return TRUE;
}
