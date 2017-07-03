#include "IOHandler.h"
IOHandler::IOHandler():m_pEvent(new IOEvent),m_bCanRead(TRUE),m_pConnResetProc(new ConnectionResetProccessor(this)),m_bCanWrite(TRUE),cs_(new CriticalSection())
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
			if(!IsServer())
				SetCanRead(FALSE);
			GetMasterThread()->InsertTask(GetRecvTask());
		}
		else
		{
			GetEvent()->SetInReady();
		}
	}
	if(events & EPOLLOUT)
	{
		if(m_bCanWrite)
		{
			GetMasterThread()->InsertTask(GetSendTask());
		}
		else
		{
			GetEvent()->SetOutReady();
		}
	}
	if(events & EPOLLERR)
	{
		//GetMasterThread()->InsertTask(m_pConnResetProc);
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

void IOHandler::SetCanWrite(int flag)
{
	m_bCanWrite = flag;
}

void IOHandler::SetCanRead(int flag)
{
	m_bCanRead = flag;
}
int IOHandler::CanRead()
{
	return m_bCanRead;
}
int IOHandler::CanWrite()
{
	return m_bCanWrite;
}

int IOHandler::LockSendBuffer()
{
	//cs_->Enter();
	return TRUE;
}


int IOHandler::UnlockSendBuffer()
{
	//cs_->Leave();
	return TRUE;
}

Task* IOHandler::GetRecvTask()
{
	ReceiveProccessor* task = new ReceiveProccessor(this);
	task->CancelRepeatable();
	return task;
}

Task* IOHandler::GetSendTask()
{
	SendProccessor* task = new SendProccessor(this);
	task->CancelRepeatable();
	return task;
}

int IOHandler::GetSide()
{
	return REMOTE_SIDE;
}

Stream* IOHandler::GetSendStream()
{
				return NULL;
}
