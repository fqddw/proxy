#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "NetUtils.h"
#include "unistd.h"
#include "string.h"
#include "netinet/tcp.h"
#include "AdminClient.h"
#include "AuthManager.h"
#include "NetEngineTask.h"
#include "DNSCache.h"
#include "ServerConfig.h"

extern DNSCache* g_pDNSCache;
extern ServerConfigDefault* g_pServerConfig;
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
	m_iState(STATUS_IDLE),
	m_iUseCount(0),
	m_pPartData(NULL),
	m_iVersion(0),
	m_iOffset(0),
	m_iContextState(VERSION)
{
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
	SetServiceType(SERVICE_TYPE_ADMIN);
}


int AdminClient::ProccessSend()
{
	if(GetEvent()->GetEventInt() & EPOLLOUT)
		GetEvent()->ModEvent(EPOLLIN);
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

	int offset = 0;
	while(1)
	{
	switch(m_iContextState)
	{
		case VERSION:
			if(pStream->GetLength() - (m_iOffset +offset) >= 4)
			{
				if(m_iOffset == 0)
				{
					m_iVersion = *(int*)pStream->GetData();
					offset+=4;
					m_iOffset = 0;
				}
				else
				{
					m_pPartData->Append(pStream->GetData() + offset, 4-m_iOffset);
					offset += 4-m_iOffset;
					m_iOffset = 0;
				}
				if(m_pPartData)
					m_pPartData->Clear();
				m_iContextState = LENGTH;
			}
			else
			{
				if(!m_pPartData)
				{
					m_pPartData = new Stream();
				}
				m_pPartData->Append(pStream->GetData()+offset, pStream->GetLength()-offset);
				return 0;
			}
			break;
		case LENGTH:
			if(pStream->GetLength() - (m_iOffset +offset) >= 4)
			{
				if(m_iOffset == 0)
				{
					m_iLength = *(int*)(pStream->GetData()+offset);
					offset+=4;
					m_iOffset = 0;
				}
				else
				{
					m_pPartData->Append(pStream->GetData() + offset, 4-m_iOffset);
					m_iLength = *(int*)m_pPartData->GetData();
					offset += 4-m_iOffset;
					m_iOffset = 0;
				}
				if(m_pPartData)
					m_pPartData->Clear();
				m_iContextState = CMD;
			}
			else
			{
				if(!m_pPartData)
				{
					m_pPartData = new Stream();
				}
				m_pPartData->Append(pStream->GetData()+offset, pStream->GetLength()-offset);
				return 0;
			}
			break;
		case CMD:
			if(m_iOffset < m_iLength)
			{
				if(!m_pPartData)
					m_pPartData = new Stream();
				int restLen = (pStream->GetLength()-offset + m_iOffset);
				if(restLen > m_iLength)
				{
					DeleteSendTask();
					delete pStream;
					ProccessConnectionClose();
					return 0;
				}
				else
				{
					m_pPartData->Append(pStream->GetData()+offset, restLen);
					offset += restLen;
					m_iOffset += restLen;
				}
			}
			if(m_iOffset == m_iLength)
			{
				m_iContextState = VERSION;
				m_iOffset = 0;
				char* pData = m_pPartData->GetData();
				int cmd = *(int*)pData;
				if(cmd == CMD_GET_AUTH_TOKEN)
				{
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
					return 0;
				}

				if(cmd == CMD_LOAD_MODULE)
				{
				}
				if(cmd == CMD_GET_DNS_LIST)
				{
					MemList<DNSItem*>* pList = g_pDNSCache->GetList();
					MemNode<DNSItem*>* pNode = pList->GetHead();
					int dataLen = 4;
					MemNode<DNSItem*>* pTmpNode = pNode;
					while(pTmpNode)
					{
						int len = strlen(pTmpNode->GetData()->GetHostName());
						dataLen+=len+4;
						pTmpNode = pTmpNode->GetNext();
					}
					char* pData = new char[dataLen];
					int offset = 0;
					memcpy(pData+offset, &dataLen, sizeof(int));
					offset+=4;
					pTmpNode = pNode;
					while(pTmpNode)
					{
						char* pHostName = pTmpNode->GetData()->GetHostName();
						int len = strlen(pHostName);
						memcpy(pData+offset, &len, sizeof(int));
						offset += sizeof(int);
						memcpy(pData+offset, pHostName, len);
						offset += len;
						pTmpNode = pTmpNode->GetNext();
					}
					m_pSendStream->Clear();
					m_pSendStream->Append(pData, dataLen);
					NetEngineTask::getInstance()->GetNetEngine()->Lock();
					NetEngineTask::getInstance()->GetNetEngine()->IncTaskCount();
					NetEngineTask::getInstance()->GetNetEngine()->Unlock();
					Lock();
					AddRef();
					Unlock();
					GetMasterThread()->InsertTask(GetSendTask());
					return 0;
				}
				if(cmd == CMD_GET_DB_CONF)
				{
					Stream* pResponse = new Stream();
					pResponse->Append("{host: ");
					pResponse->Append(g_pServerConfig->GetDBHost());
					pResponse->Append(",user: ");
					pResponse->Append(g_pServerConfig->GetDBUsername());
					pResponse->Append(",password: ");
					pResponse->Append(g_pServerConfig->GetDBPassword());
					pResponse->Append(",port: ");
					char portString[16] = {'\0'};
					sprintf(portString, "%d", g_pServerConfig->GetDBPort());
					pResponse->Append(portString);
					pResponse->Append("}");
					m_pSendStream->Clear();
					m_pSendStream->Append(pResponse);
					delete pResponse;
					NetEngineTask::getInstance()->GetNetEngine()->Lock();
					NetEngineTask::getInstance()->GetNetEngine()->IncTaskCount();
					NetEngineTask::getInstance()->GetNetEngine()->Unlock();
					Lock();
					AddRef();
					Unlock();
					GetMasterThread()->InsertTask(GetSendTask());
					return 0;
				}

			}
			break;
		default:
			;
	}
	}
	delete pStream;
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

