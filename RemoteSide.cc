#include "RemoteSide.h"
#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
#include "errno.h"
#include "NetUtils.h"
#include "unistd.h"
#include "string.h"
#include "QueuedNetTask.h"

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;
extern MemList<void*>* pGlobalList;
int RemoteSide::Proccess()
{
	return TRUE;
}

int RemoteSide::Writeable()
{
	return TRUE;
}
int RemoteSide::IsIdle()
{
	return m_iState == STATUS_IDLE;
}
int RemoteSide::SetStatusBlocking()
{
	m_iState = STATUS_BLOCKING; 
	return TRUE;
}

int RemoteSide::SetStatusIdle()
{
	delete m_pHttpResponse;
	m_pHttpResponse = new HttpResponse(m_pStream);
	m_pClientSide = NULL;

	SetMainTask(NULL);
	m_iState = STATUS_IDLE; 
	return TRUE;
}

InetSocketAddress* RemoteSide::GetAddr()
{
	return m_pAddr;
}
RemoteSide::RemoteSide():
	IOHandler(),
	m_pSendStream(new Stream),
	m_pStream(new Stream),
	m_iState(STATUS_BLOCKING),
	m_isConnected(FALSE),
	m_iClientState(STATE_NORMAL),
	m_bCloseClient(FALSE),
	m_bSSL(FALSE)
{
	m_iSide = REMOTE_SIDE;
	GetEvent()->SetIOHandler(this);
}
RemoteSide::RemoteSide(InetSocketAddress* pAddr):
	IOHandler(),
	m_pSendStream(new Stream),
	m_pStream(new Stream),
	m_iState(STATUS_BLOCKING),
	m_isConnected(FALSE),
	m_iClientState(STATE_NORMAL),
	m_bCloseClient(FALSE),
	m_bShouldClose(FALSE),
	m_bSSL(FALSE)
{
	m_iSide = REMOTE_SIDE;
	GetEvent()->SetIOHandler(this);
	m_pAddr = pAddr;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	m_iSocket = sockfd;
	int cflags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL, cflags|O_NONBLOCK);
	GetEvent()->SetFD(sockfd);
	m_pHttpResponse = new HttpResponse(m_pStream);
}
int RemoteSide::Connect()
{
	struct sockaddr sa = m_pAddr->ToSockAddr();
	m_isConnected = SOCKET_STATUS_CONNECTING;
	int ret = connect(m_iSocket,&sa,sizeof(sa));
	GetEvent()->ModEvent(EPOLLOUT|/*EPOLLET|*/EPOLLONESHOT);
	return ret;
}
int RemoteSide::ProccessSend()
{
	if(m_pSendStream->GetLength() == 0)
	{
	/*
		return FALSE;
	*/
	}

	/*if(!m_pClientSide)
	  return FALSE;*/
	//处理连接操作
	if(m_isConnected == SOCKET_STATUS_PRE_CONNECTING)
	{
		m_isConnected = SOCKET_STATUS_CONNECTING;
		return TRUE;
	}
	if(m_isConnected == SOCKET_STATUS_CONNECTING)
	{
		int error = 0;
		socklen_t size = sizeof(int);
		getsockopt(GetEvent()->GetFD(), SOL_SOCKET, SO_ERROR, (char*)&error, &size);
		if(error != 0)
		{
			//printf("Not Connected %s\n", m_pClientSide->GetStream()->GetData());
			//GetEvent()->ModEvent(EPOLLIN|EPOLLET);
			SetClosed(TRUE);
			//if(m_pClientSide)
				//m_pClientSide->ProccessConnectionReset();
			ProccessConnectionReset();
			return 0;
		}
		m_isConnected = TRUE;
		if(m_pSendStream->GetLength())
		{
		}
		m_isConnected = TRUE;
		GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
		//m_pClientSide->GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
		if(m_bSSL)
		{
			if(!m_pClientSide)
			{
				ProccessConnectionReset();
				return 0;
			}
			if(m_iClientState != STATE_RUNNING)
			{
			}
			m_pClientSide->GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
			const char* pConnEstablished= "HTTP/1.1 200 Connection Established\r\nContent-Length: 0\r\n\r\n";
			int len = strlen(pConnEstablished);
			m_pClientSide->GetSendStream()->Append((char*)pConnEstablished, len);
			m_pClientSide->SetSendFlag();
			LockTask();
			if(!GetMainTask()->IsRunning())
				GetMasterThread()->InsertTask(GetMainTask());
			UnlockTask();
			//GetMasterThread()->InsertTask(m_pClientSide->GetSendTask());
			return 0;
		}
	}
	if(!IsConnected())
	{
		Connect();
		return FALSE;
	}
	if(GetEvent()->GetEventInt() & EPOLLOUT)
	{
		GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
	}

	int totalSend = 0;
	int flag = TRUE;
	while(flag)
	{
		int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
		if(nSent == -1)
		{
			flag = FALSE;
			if(errno == EAGAIN)
			{
				GetEvent()->ModEvent(EPOLLOUT|/*EPOLLET|*/EPOLLONESHOT);
			}
			else
			{
				return 0;
			}
		}
		else
		{
			totalSend += nSent;
			m_pSendStream->Sub(nSent);
			if(m_pSendStream->GetLength() == 0)
			{
				if(!(m_pClientSide->GetEvent()->GetEventInt() & EPOLLOUT))
					m_pClientSide->GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
				return 0;
			}
		}
	}
	return TRUE;
}

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;
int RemoteSide::ClearHttpEnd()
{
	delete m_pHttpResponse;
	m_pHttpResponse = new HttpResponse(m_pStream);
	m_pStream->Sub(m_pStream->GetLength());
	return 0;

}
int RemoteSide::ProccessReceive(Stream* pStream)
{
	Stream* pSendStream = NULL;
	if(!GetMainTask())
	{
		ProccessConnectionClose();
		return 0;
	}
	if(!pStream)
	{
		ProccessConnectionClose();
		return TRUE;
	}
	if(!m_pClientSide)
	{
		delete pStream;
		ProccessConnectionReset();
		return 0;
	}
	Stream* pUserStream = pStream;
	if(m_pClientSide == NULL)
	{
		delete pUserStream;
		return 0;
	}
	if(m_bSSL)
	{
		//printf("received data %s\n", m_pClientSide->GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetHost());
		int iLength = m_pClientSide->GetSendStream()->GetLength();
		m_pClientSide->GetSendStream()->Append(pStream->GetData(), pStream->GetLength());
		/*if(m_pClientSide && iLength == 0GetSendRefCount() == 0 && !m_pClientSide->IsRealClosed())
		{
			GetMasterThread()->InsertTask(m_pClientSide->GetSendTask());
		}
		else
		{
			printf("Have Length\n");
		}*/
		m_pClientSide->SetSendFlag();
		LockTask();
		if(!GetMainTask()->IsRunning())
			GetMasterThread()->InsertTask(GetMainTask());
		UnlockTask();
		delete pStream;
		if(IsClosed())
		{
			ProccessConnectionClose();
		}

		return 0;
	}
	int isEnd = TRUE;
	if(m_pHttpResponse->GetState() == HEADER_NOTFOUND)
	{
		m_pStream->Append(pStream->GetData(),pStream->GetLength());
		int iHeaderSize = m_pHttpResponse->IsHeaderEnd();
		if(iHeaderSize)
		{
			m_pHttpResponse->SetState(HEADER_FOUND);
			m_pHttpResponse->LoadHttpHeader();
			m_pHttpResponse->GetHeader()->DeleteField((char*)"Set-Cookie");
			pSendStream = m_pHttpResponse->GetHeader()->ToHeader();
			char* pConnection = m_pHttpResponse->GetHeader()->GetField(HTTP_CONNECTION);
			if(pConnection)
			{
				if(strstr(pConnection, "close"))
				{
					m_bShouldClose = TRUE;
				}
				if(strstr(pConnection,"close") && m_pHttpResponse->GetHeader()->GetField(HTTP_CONTENT_LENGTH) == NULL && m_pHttpResponse->GetHeader()->GetField(HTTP_TRANSFER_ENCODING) == NULL)
				{
					m_bCloseClient = TRUE;
					m_pClientSide->SetCloseAsLength(TRUE);
				}
			}
			if(m_pHttpResponse->HasBody())
			{
				isEnd = FALSE;
				m_pHttpResponse->LoadBody();
				Stream* pBodyStream = m_pStream->GetPartStream(iHeaderSize,m_pStream->GetLength());
				if(pBodyStream)
				{
					isEnd = m_pHttpResponse->GetBody()->IsEnd(pBodyStream);
					pSendStream->Append(pBodyStream);
					delete pBodyStream;
				}
			}
			m_pStream->Sub(m_pStream->GetLength());
		}
		else
		{
			delete pUserStream;
			GetEvent()->ModEvent(EPOLLIN|EPOLLONESHOT);

			isEnd = FALSE;
			return FALSE;
		}
	}
	else
	{
		if(m_pHttpResponse->HasBody())
			if(m_pHttpResponse->GetBody())
				isEnd = m_pHttpResponse->GetBody()->IsEnd(pUserStream);
	}
	{

		if(m_iClientState != STATE_RUNNING)
		{
			ClearHttpEnd();
			ProccessConnectionReset();
			if(pSendStream)
				delete pSendStream;
			delete pUserStream;
			return 0;
		}

		int nLengthSend = m_pClientSide->GetSendStream()->GetLength();
		m_pClientSide->LockSendBuffer();
		if(pSendStream)
		{
			m_pClientSide->GetSendStream()->Append(pSendStream->GetData(),pSendStream->GetLength());
			delete pSendStream;
		}
		else
		{
			m_pClientSide->GetSendStream()->Append(pUserStream->GetData(),pUserStream->GetLength());
		}
		delete pUserStream;
		m_pClientSide->UnlockSendBuffer();
		ClientSide* pClientSide = m_pClientSide;
		QueuedNetTask* pMainTask = GetMainTask();
		if(isEnd)
		{
			//如果拉取信息结束,则解耦
			if(m_iClientState == STATE_RUNNING)
			{
				m_iClientState = STATE_NORMAL;
				m_pClientSide->SetRemoteState(STATE_NORMAL);

				ClearHttpEnd();
				if(!m_bShouldClose)
				{
					SetStatusIdle();
					GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
				}
				else
					SetClosed(TRUE);
			}
		}

		/*
		if(nLengthSend == 0)
		{
			//printf("Multi Thread RecvTask %s %d\n", __FILE__, __LINE__);
			GetMasterThread()->InsertTask(pClientSide->GetSendTask());
			//pClientSide->ProccessSend();
		}
		else
		{
		}*/
		pClientSide->SetSendFlag();
		pMainTask->Lock();
		if(!pMainTask->IsRunning())
			GetMasterThread()->InsertTask(pMainTask);
		pMainTask->Unlock();
		/*if(IsClosed())
		{
			ProccessConnectionClose();
		}*/


	}

	return TRUE;
}

Stream* RemoteSide::GetSendStream()
{
	return m_pSendStream;
}

int RemoteSide::SetSendStream(Stream* pStream)
{
	m_pSendStream = pStream;
	return TRUE;
}

int RemoteSide::SetClientSide(ClientSide* pClientSide)
{
	m_pClientSide = pClientSide;
	return TRUE;
}
HttpResponse* RemoteSide::GetResponse()
{
	return m_pHttpResponse;
}

int RemoteSide::IsConnected()
{
	return m_isConnected == TRUE;
}

int RemoteSide::ProccessConnectionReset()
{
	if(IsRealClosed())
	{
		return 0;
	}
	SetRealClosed(TRUE);

	if(IsIdle())
	{
	}
	else
	{
		if(m_iClientState != STATE_ABORT)
		{
			if(m_pClientSide)
			{
				m_pClientSide->SetRemoteState(STATE_ABORT);
				m_iClientState = STATE_NORMAL;
				ClearHttpEnd();
				//printf("Multi Thread RecvTask %s %d\n", __FILE__, __LINE__);
				m_pClientSide->ProccessConnectionReset();
				//GetMasterThread()->InsertTask(m_pClientSide->GetRecvTask());
				m_pClientSide = NULL;
			}
		}
	}
	int sockfd = GetEvent()->GetFD();
	GetEvent()->RemoveFromEngine();
	g_pGlobalRemoteSidePool->Delete(this);
	close(sockfd);
	if(GetMainTask())
		GetMainTask()->SetRemote(NULL);

	Release();
	return 0;
}
int RemoteSide::ProccessConnectionClose()
{
	if(IsRealClosed())
	{
		return 0;
	}
	SetRealClosed(TRUE);

	if(IsIdle())
	{
	}
	else
	{
		if(m_iClientState != STATE_ABORT && m_iClientState != STATE_NORMAL)
		{
			if(m_pClientSide)
			{
				m_pClientSide->SetRemoteState(STATE_NORMAL);
				m_pClientSide->SetRemoteSide(NULL);
				m_pClientSide->SetCloseAsLength(TRUE);
				m_iClientState = STATE_NORMAL;
				ClearHttpEnd();
				//printf("Multi Thread RecvTask %s %d %s\n", __FILE__, __LINE__, m_pClientSide->GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetHost());
				if((m_bCloseClient || m_bSSL) && m_pClientSide->GetSendStream()->GetLength() == 0);
				//if(m_bSSL)
					m_pClientSide->ProccessConnectionReset();
				//GetMasterThread()->InsertTask(m_pClientSide->GetRecvTask());
				//m_pClientSide = NULL;
			}
		}
	}
	int sockfd = GetEvent()->GetFD();
	GetEvent()->RemoveFromEngine();
	g_pGlobalRemoteSidePool->Delete(this);
	close(sockfd);
	if(GetMainTask())
		GetMainTask()->SetRemote(NULL);

	Release();

	return 0;
}

int RemoteSide::GetSide()
{
	return REMOTE_SIDE;
}
RemoteSide::~RemoteSide()
{
	delete m_pStream;
	m_pStream = NULL;
	delete m_pSendStream;
	m_pSendStream = NULL;
	if(m_pHttpResponse)
	{
		delete m_pHttpResponse;
		m_pHttpResponse = NULL;
	}
	delete m_pAddr;
}

void RemoteSide::SetClientState(int iState)
{
	m_iClientState = iState;
}

void RemoteSide::EnableSSL()
{
	m_bSSL = TRUE;
	m_bCloseClient = TRUE;
}

void RemoteSide::SetRecvFlag()
{
	GetMainTask()->SetRemoteRecving();
}


void RemoteSide::SetSendFlag()
{
	GetMainTask()->SetRemoteSending();
}


void RemoteSide::SetMainTask(QueuedNetTask* pTask)
{
	IOHandler::SetMainTask(pTask);
	if(pTask)
		pTask->SetRemote(this);
}
