#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
#include "errno.h"
#include "NetUtils.h"
#include "unistd.h"
#include "string.h"
#include "netinet/tcp.h"
#include "AdminClient.h"
#include "AuthManager.h"
#include "NetEngineTask.h"

int AdminClient::IsIdle()
{
	return m_iState == STATUS_IDLE;
}
int AdminClient::SetStatusBlocking()
{
	m_iState = STATUS_BLOCKING; 
	return TRUE;
}

int AdminClient::SetStatusIdle()
{
	m_iSentTotal = 0;
	m_iRecvTotal = 0;
	m_pSendStream->Clear();
	SetEndTime(Time::GetNow());
	SetMainTask(NULL);
	m_iState = STATUS_IDLE; 
	return TRUE;
}

AdminClient::AdminClient():
	IOHandler(),
	m_pSendStream(new Stream),
	m_pStream(new Stream),
	m_iState(STATUS_BLOCKING),
	m_isConnected(FALSE),
	m_iClientState(STATE_NORMAL),
	m_bCloseClient(FALSE),
	m_iSentTotal(0),
	m_iRecvTotal(0),
	m_iUseCount(0)
{
	m_iSide = REMOTE_SIDE;
	GetEvent()->SetIOHandler(this);
	SetServiceType(SERVICE_TYPE_ADMIN);
}

AdminClient::AdminClient(int sockfd):
	IOHandler(),
	m_pStream(new Stream()),
	m_pSendStream(new Stream()),
	m_iSentTotal(0),
	m_iRecvTotal(0),
	m_iUseCount(0)
{
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
	SetServiceType(SERVICE_TYPE_ADMIN);
}


int AdminClient::ProccessSend()
{
	int nSent = send(GetEvent()->GetFD(), m_pSendStream->GetData()+m_iSentTotal, m_pSendStream->GetLength()-m_iSentTotal, 0);
	if(nSent > 0)
	{
		m_iSentTotal += nSent;
		if(m_iSentTotal == m_pSendStream->GetLength())
		{
			m_pSendStream->Clear();
			m_iSentTotal = 0;
		}
	}
	else
	{
		GetEvent()->ModEvent(EPOLLOUT);
	}
	return TRUE;
}


int AdminClient::ProccessReceive(Stream* pStream)
{
	if(!pStream)
	{
		DeleteSendTask();
		ProccessConnectionClose();
		return 0;
	}
	Auth* pAuth = AuthManager::getInstance()->GenerateAuthToken();
	int len = 32*2+4*2;
	int nonceLen = 32;
	int opaqueLen = 32;
	char* pBuffer = new char[len];
	memcpy(pBuffer, &nonceLen, 4);
	memcpy(pBuffer+4, pAuth->GetNonce()->GetData(), pAuth->GetNonce()->GetLength());
	memcpy(pBuffer +4 + 32, &nonceLen, 4);
	memcpy(pBuffer + 4+ 32 + 4, pAuth->GetOpaque()->GetData(), pAuth->GetOpaque()->GetLength());
	m_pSendStream->Clear();
	m_pSendStream->Append(pBuffer, len);
	NetEngineTask::getInstance()->GetNetEngine()->Lock();
	NetEngineTask::getInstance()->GetNetEngine()->IncTaskCount();
	NetEngineTask::getInstance()->GetNetEngine()->Unlock();
	Lock();
	AddRef();
	Unlock();
	GetMasterThread()->InsertTask(GetSendTask());
	return TRUE;
}

int AdminClient::IsConnected()
{
	return m_isConnected == TRUE;
}

int AdminClient::ProccessConnectionReset()
{
	return 0;
}
int AdminClient::ProccessConnectionClose()
{
	int sockfd = GetEvent()->GetFD();
	GetEvent()->RemoveFromEngine();
	close(sockfd);

	Release();
	return 0;
}

AdminClient::~AdminClient()
{
	delete m_pStream;
	m_pStream = NULL;
	delete m_pSendStream;
	m_pSendStream = NULL;
}

void AdminClient::IncUseCount()
{
	m_iUseCount++;
}

void AdminClient::SetStartTime(struct timespec tCurTime)
{
	start_time = tCurTime;
}

void AdminClient::SetEndTime(struct timespec tCurTime)
{
	end_time = tCurTime;
}

Stream* AdminClient::GetSendStream(){
	return m_pSendStream;
}

