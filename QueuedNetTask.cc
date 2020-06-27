#include "QueuedNetTask.h"
#include "errno.h"
#include "NetEngineTask.h"
#include "DNSCache.h"
#include "arpa/inet.h"
#include <netinet/in.h>
#include "unistd.h"

extern DNSCache* g_pDNSCache;
int QueuedNetTask::Run()
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
					m_pRemoteSide->ProccessConnectionClose();
				}
				if(m_pDNS == NULL)
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
			case DNS_ARRIVING:
				{
					Stream* pStream = NULL;
					GetDataGRAM(m_pDNS, &pStream);
					if(pStream)
					{
						int ip = m_pDNS->ProccessReceive(pStream);
						if(ip)
						{
							g_pDNSCache->AddRecord(m_pDNS->getUrl(), ip, TRUE);
							if(m_pClientSide)
								m_pClientSide->AfterDNS(ip);
							else
								CancelRepeatable();
						}
					}
					m_pDNS->GetEvent()->RemoveFromEngine();
					close(m_pDNS->GetEvent()->GetFD());
					delete m_pDNS;
					m_pDNS=NULL;
				}
				break;
			default:
				;

		}
	}
	return 0;
}


int QueuedNetTask::GetDataStream(IOHandler* pIOHandler, Stream** ppStream)
{
	int sockfd = pIOHandler->GetEvent()->GetFD();
	char* buffer = new char[1024*256];
	int flag = TRUE;
	int total = 0;
	while(flag)
	{
		flag = FALSE;
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
	return 0;
}
int QueuedNetTask::GetDataGRAM(IOHandler* pIOHandler, Stream** ppStream)
{
	int sockfd = pIOHandler->GetEvent()->GetFD();
	char* buffer = new char[1024];
	int flag = TRUE;
	int total = 0;
	//while(flag)
	{
		flag = FALSE;
	struct sockaddr_in sai={0};

	sai.sin_family = AF_INET;
	sai.sin_addr.s_addr = inet_addr("119.29.29.29");
	sai.sin_port = htons(53);
	socklen_t len=sizeof(sai);

		int n = recvfrom(sockfd, buffer, 1024, 0,(struct sockaddr*)&sai,&len);
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
	return 0;
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
	if(m_bDNSArriving)
	{
		m_bDNSArriving = FALSE;
		return DNS_ARRIVING;
	}

	return FALSE;
}


QueuedNetTask::QueuedNetTask():Task()
{
m_pClientSide = (NULL);
m_pRemoteSide = (NULL);m_bClientRecving = (FALSE);m_bClientSending = (FALSE);m_bRemoteRecving = (FALSE);m_bRemoteSending = (FALSE),cs_ = (new CriticalSection());m_bRunning = (FALSE);m_iCount = (1);
m_bDNSArriving = FALSE;
}

QueuedNetTask::~QueuedNetTask()
{
	cs_->Enter();
	m_iCount--;
	cs_->Leave();
	delete cs_;
	cs_ = NULL;
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
void QueuedNetTask::SetDNS(DNSFetch* pDNS)
{
	m_pDNS = pDNS;
}


void QueuedNetTask::SetClientRecving()
{
	m_bClientRecving = TRUE;
}

void QueuedNetTask::SetClientSending()
{
	m_bClientSending = TRUE;
}

void QueuedNetTask::SetRemoteRecving()
{
	m_bRemoteRecving = TRUE;
}


void QueuedNetTask::SetRemoteSending()
{
	m_bRemoteSending = TRUE;
}
void QueuedNetTask::SetDNSArriving()
{
	m_bDNSArriving = TRUE;
}

void QueuedNetTask::SetRunning()
{
	m_bRunning = TRUE;
}

int QueuedNetTask::IssetClientRecving()
{
	return m_bClientRecving;
}

int QueuedNetTask::IssetRemoteRecving()
{
	return m_bRemoteRecving;
}

void QueuedNetTask::IncCount()
{
	m_iCount=m_iCount+1;
}

ClientSide* QueuedNetTask::GetClientSide()
{
	return m_pClientSide;
}

RemoteSide* QueuedNetTask::GetRemoteSide()
{
	return m_pRemoteSide;
}


