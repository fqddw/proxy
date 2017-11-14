#include "QueuedNetTask.h"
#include "errno.h"
int QueuedNetTask::Run()
{
	int flag = TRUE;
	while(flag)
	{
		cs_->Enter();
		int task = GetNextTask();
		if(!task)
		{
			m_bRunning = FALSE;
			if(m_pClientSide == NULL && m_pRemoteSide == NULL)
			{
				CancelRepeatable();
			}
			cs_->Leave();
			return 0;
		}
		cs_->Leave();
		switch(task)
		{
			case CLIENT_RECVING:
				{
					if(!m_pClientSide)
						continue;
					Stream* pStream = NULL;
					GetDataStream(m_pClientSide, &pStream);
					m_pClientSide->ProccessReceive(pStream);
				}
				break;
			case CLIENT_SENDING:
				{
					if(!m_pClientSide)
						continue;

					m_pClientSide->ProccessSend();
				}
				break;
			case REMOTE_RECVING:
				{
					if(!m_pRemoteSide)
						continue;

					Stream* pStream = NULL;
					GetDataStream(m_pRemoteSide, &pStream);
					m_pRemoteSide->ProccessReceive(pStream);

				}
				break;
			case REMOTE_SENDING:
				{
					if(!m_pRemoteSide)
						continue;

					m_pRemoteSide->ProccessSend();
				}
				break;
			default:
				;

		}
	}
}


int QueuedNetTask::GetDataStream(IOHandler* pIOHandler, Stream** ppStream)
{
	int sockfd = pIOHandler->GetEvent()->GetFD();
	char* buffer = new char[1024*256];
	int flag = TRUE;
	int total = 0;
	while(flag)
	{
		int n = recv(sockfd, buffer, 256*1024, 0);
		if(n > 0)
		{
			total+=n;
			if(!*ppStream)
				*ppStream = new Stream();
			(*ppStream)->Append(buffer, n);
		}
		else
		{
			flag = FALSE;
			if(n == -1)
			{
				printf("%d %d %d %d\n", errno, pIOHandler->GetSide(), pIOHandler->GetEvent()->GetFD(), pIOHandler->GetEvent()->GetEventInt());
			}
		}
	}
	delete buffer;
}
int QueuedNetTask::GetNextTask()
{
	if(m_bClientRecving)
	{
		m_bClientRecving = FALSE;
		return CLIENT_RECVING;
	}

	if(m_bClientSending)
	{
		m_bClientSending = FALSE;
		return CLIENT_SENDING;
	}

	if(m_bRemoteRecving)
	{
		m_bRemoteRecving = FALSE;
		return REMOTE_RECVING;
	}

	if(m_bRemoteSending)
	{
		m_bRemoteSending = FALSE;
		return REMOTE_SENDING;
	}
	return FALSE;
}


QueuedNetTask::QueuedNetTask():Task(),m_pClientSide(NULL),m_pRemoteSide(NULL),m_bClientRecving(FALSE),m_bClientSending(FALSE),m_bRemoteRecving(FALSE),m_bRemoteSending(FALSE),cs_(new CriticalSection()),m_bRunning(FALSE)
{
}

void QueuedNetTask::Lock()
{
	cs_->Enter();
}


void QueuedNetTask::Unlock()
{
	cs_->Leave();
}

int QueuedNetTask::IsRunning()
{
	return m_bRunning;
}


void QueuedNetTask::SetClient(ClientSide* pSide)
{
	m_pClientSide = pSide;
}

void QueuedNetTask::SetRemote(RemoteSide* pSide)
{
	m_pRemoteSide = pSide;
}

void QueuedNetTask::SetClientRecving()
{
	cs_->Enter();
	m_bClientRecving = TRUE;
	cs_->Leave();
}

void QueuedNetTask::SetClientSending()
{
	cs_->Enter();
	m_bClientSending = TRUE;
	cs_->Leave();
}

void QueuedNetTask::SetRemoteRecving()
{
	cs_->Enter();
	m_bRemoteRecving = TRUE;
	cs_->Leave();
}


void QueuedNetTask::SetRemoteSending()
{
	cs_->Enter();
	m_bRemoteSending = TRUE;
	cs_->Leave();
}

void QueuedNetTask::SetRunning()
{
	m_bRunning = TRUE;
}
