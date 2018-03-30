#include "NetTask.h"
#include "errno.h"
#include "NetEngineTask.h"
int NetTask::Run()
{
	int flag = TRUE;
	while(flag)
	{
		cs_->Enter();
		int task = GetNextTask();
		if(!task)
		{
			m_iCount--;
			m_bRunning = FALSE;
			if(m_pClientSide == NULL/* && m_pRemoteSide == NULL*/)
			{
				if(m_pRemoteSide && m_pRemoteSide->GetMainTask())
				{
					m_pRemoteSide->SetMainTask(NULL);
				}
				CancelRepeatable();
			}
			cs_->Leave();
			int bRepeatable = Repeatable();
			NetEngineTask::getInstance()->GetNetEngine()->Lock();
			NetEngineTask::getInstance()->GetNetEngine()->ReduceTaskCount();
			int iTaskCount = NetEngineTask::getInstance()->GetNetEngine()->GetTaskCount();
			if(iTaskCount == 0)
			{
				NetEngineTask::getInstance()->IncCount();
				NetEngineTask::getInstance()->GetNetEngine()->GetMasterThread()->InsertTask(NetEngineTask::getInstance());
			}
			NetEngineTask::getInstance()->GetNetEngine()->Unlock();
			return bRepeatable;
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
					if(pStream)
						delete pStream;
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
					if(pStream)
						delete pStream;
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


int NetTask::GetDataStream(IOHandler* pIOHandler, Stream** ppStream)
{
	int sockfd = pIOHandler->GetEvent()->GetFD();
	char* buffer = new char[1024*256];
	int flag = TRUE;
	int total = 0;
	//while(flag)
	{
		int n = recv(sockfd, buffer, 256*1024, 0);
		if(n > 0)
		{
			if(errno == EAGAIN)
			{
				//printf("%d %d\n", errno, n);
			}
			//printf("%s\n",buffer);
			total+=n;
			if(!*ppStream)
				*ppStream = new Stream();
			(*ppStream)->Append(buffer, n);
		}
		else
		{
			flag = FALSE;
			//if(n == -1)
			{
				//printf("%d %d %d %d %d\n", n, errno, pIOHandler->GetSide(), pIOHandler->GetEvent()->GetFD(), pIOHandler->GetEvent()->GetEventInt());
			}
		}
	}
	delete []buffer;
}
int NetTask::GetNextTask()
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


NetTask::NetTask():Task(),m_pClientSide(NULL),m_pRemoteSide(NULL),m_bClientRecving(FALSE),m_bClientSending(FALSE),m_bRemoteRecving(FALSE),m_bRemoteSending(FALSE),cs_(new CriticalSection()),m_bRunning(FALSE),m_iCount(1)
{
}

NetTask::~NetTask()
{
	cs_->Enter();
	m_iCount--;
	cs_->Leave();
	delete cs_;
	cs_ = NULL;
}
void NetTask::Lock()
{
	cs_->Enter();
}


void NetTask::Unlock()
{
	cs_->Leave();
}

int NetTask::IsRunning()
{
	return m_bRunning;
}


void NetTask::SetClient(ClientSide* pSide)
{
	m_pClientSide = pSide;
}

void NetTask::SetRemote(RemoteSide* pSide)
{
	m_pRemoteSide = pSide;
}

void NetTask::SetClientRecving()
{
	m_bClientRecving = TRUE;
}

void NetTask::SetClientSending()
{
	m_bClientSending = TRUE;
}

void NetTask::SetRemoteRecving()
{
	m_bRemoteRecving = TRUE;
}


void NetTask::SetRemoteSending()
{
	m_bRemoteSending = TRUE;
}

void NetTask::SetRunning()
{
	m_bRunning = TRUE;
}

int NetTask::IssetClientRecving()
{
	return m_bClientRecving;
}

int NetTask::IssetRemoteRecving()
{
	return m_bRemoteRecving;
}

void NetTask::IncCount()
{
	m_iCount=m_iCount+1;
}