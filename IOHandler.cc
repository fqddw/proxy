#include "IOHandler.h"
#include "MemList.h"
extern MemList<void*>* pGlobalList;
IOHandler::IOHandler():
								m_pEvent(new IOEvent),
								m_bCanRead(TRUE),
								m_bCanWrite(TRUE),
								cs_(new CriticalSection()),
								m_bClosed(FALSE),
								m_iRefCount(1),
								m_iSendRefCount(0),
								m_iRecvRefCount(0),
								m_bRealClosed(0),
								m_bDeleted(FALSE)
{
	pGlobalList->Append(this);
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
								AddRecvRefCount();
	ReceiveProccessor* task = new ReceiveProccessor(this);
	task->CancelRepeatable();
	return task;
}

Task* IOHandler::GetSendTask()
{
								//if(m_iRefCount > 1 && !IsServer())
										//						return NULL;
								AddRef();
								AddSendRefCount();
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
	Lock();
								m_iRefCount--;
								if(m_iRefCount < 0)
									printf("Ref %d %d\n", m_iRefCount, m_iSide);
								Unlock();
								if(m_iRefCount == 0)
								{
									pGlobalList->Delete(this);
									if(!m_bDeleted)
									{
										m_bDeleted = TRUE;
										//delete this;
									}

								}
}

int IOHandler::GetRefCount()
{
								return m_iRefCount;
}

void IOHandler::AddRecvRefCount()
{
	Lock();
	m_iRecvRefCount++;
	Unlock();
}

int IOHandler::GetRecvRefCount()
{
	return m_iRecvRefCount;
}
int IOHandler::GetSendRefCount()
{
	return m_iSendRefCount;
}
void IOHandler::AddSendRefCount()
{
	m_iSendRefCount++;
}

void IOHandler::ReleaseSendRefCount()
{
								m_iSendRefCount--;
}

void IOHandler::ReleaseRecvRefCount()
{
								m_iRecvRefCount--;
}

int IOHandler::IsRealClosed()
{
	return m_bRealClosed;
}

void IOHandler::SetRealClosed(int bRealClosed)
{
	m_bRealClosed = bRealClosed;
}

void IOHandler::Lock()
{
	cs_->Enter();
}

void IOHandler::Unlock()
{
	cs_->Leave();
}
