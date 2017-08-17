#include "IOHandler.h"
IOHandler::IOHandler():
								m_pEvent(new IOEvent),
								m_bCanRead(TRUE),
								m_bCanWrite(TRUE),
								cs_(new CriticalSection()),
								m_bClosed(FALSE),
								m_iRefCount(1)
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
	m_pEvent = NULL;
	delete cs_;
	cs_ = NULL;
}
#include "stdio.h"
int IOHandler::Dispatch(int events)
{
	if(events & EPOLLIN)
	{
		if(m_bCanRead)
		{
			GetEvent()->CancelInReady();
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
						if(!IsServer())
										SetCanWrite(FALSE);

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
								//if(m_iRefCount > 1 && !IsServer())
										//						return NULL;
								AddRef();
	ReceiveProccessor* task = new ReceiveProccessor(this);
	task->CancelRepeatable();
	return task;
}

Task* IOHandler::GetSendTask()
{
								//if(m_iRefCount > 1 && !IsServer())
										//						return NULL;
								AddRef();
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

int IOHandler::IsClosed()
{
				return m_bClosed;
}

void IOHandler::SetClosed(int bClosed)
{
				m_bClosed = bClosed;
}

void IOHandler::AddRef()
{
								m_iRefCount++;
}

void IOHandler::Release()
{
								m_iRefCount--;
								if(m_iRefCount == 0)
								{
																delete this;
								}
}

int IOHandler::GetRefCount()
{
								return m_iRefCount;
}
