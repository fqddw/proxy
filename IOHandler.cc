#include "IOHandler.h"
IOHandler::IOHandler():m_pEvent(new IOEvent),m_iStatus(HANDLER_IDLE),Task()
{
}
IOEvent* IOHandler::GetEvent()
{
	return m_pEvent;
}

int IOHandler::Run()
{
	SetRunningStatus(HANDLER_RUNNING);
	Proccess();
	SetRunningStatus(HANDLER_IDLE);
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
int IOHandler::GetRunningStatus()
{
	return m_iStatus;
}

void IOHandler::SetRunningStatus(int status)
{
	m_iStatus = status;
}

int IOHandler::Dispatch()
{
	if(GetRunningStatus() == HANDLER_IDLE)
	{
		SetRunningStatus(HANDLER_RUNNING);
		GetMasterThread()->InsertTask(this);
	}
	return TRUE;
}

int IOHandler::Proccess()
{
	return TRUE;
}
