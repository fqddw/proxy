#include "RemoteSide.h"
#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
#include "errno.h"
#include "NetUtils.h"
#include "unistd.h"
#include "string.h"

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

	m_iState = STATUS_BLOCKING; 
	SetCanRead(TRUE);
	return TRUE;
}

InetSocketAddress* RemoteSide::GetAddr()
{
	return m_pAddr;
}
RemoteSide::RemoteSide():IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream),m_iState(STATUS_BLOCKING),m_isConnected(FALSE),m_bCloseClient(FALSE)
{
	GetEvent()->SetIOHandler(this);
}
RemoteSide::RemoteSide(InetSocketAddress* pAddr):IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream),m_iState(STATUS_BLOCKING),m_isConnected(FALSE),m_bCloseClient(FALSE)
{
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
	m_isConnected = SOCKTE_STATUS_CONNECTING;
	int ret = connect(m_iSocket,&sa,sizeof(sa));
	return ret;
}
int RemoteSide::ProccessSend()
{
	if(m_isConnected == SOCKTE_STATUS_CONNECTING)
	{
		if(m_pSendStream->GetLength())
			SetCanWrite(TRUE);
		m_isConnected = TRUE;
		GetEvent()->ModEvent(EPOLLIN|EPOLLET);
	}
	if(!IsConnected())
	{
		Connect();
		return FALSE;
	}

		int totalSend = 0;
		int flag = TRUE;
		while(flag)
		{
			LockSendBuffer();
			int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
			if(nSent == -1)
			{
				flag = FALSE;
				if(errno == EAGAIN)
					printf("--------------------------\n");
				else if(errno == EINTR)
					printf("||||||||||||||||||\n");
				else
				{
					printf("%d +++++++++++++++++++\n",errno);
					GetEvent()->RemoveFromEngine();
					close(GetEvent()->GetFD());
					return 0;
				}
			}else if(nSent == 0)
			{
				GetEvent()->RemoveFromEngine();
				close(GetEvent()->GetFD());
				return 0;
			}
			else
			{
				totalSend += nSent;
				m_pSendStream->Sub(nSent);
				if(m_pSendStream->GetLength() == 0)
				{
					flag = FALSE;
					SetCanWrite(flag);
					m_pClientSide->SetCanRead(TRUE);
				}
			}
			UnlockSendBuffer();
		}
	return TRUE;
}

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;
int RemoteSide::ProccessReceive(Stream* pStream)
{
	if(!pStream)
	{
		GetEvent()->CancelInReady();
		SetCanRead(TRUE);
		return TRUE;
	}
	if(!m_pClientSide)
	{
		GetEvent()->RemoveFromEngine();
		close(GetEvent()->GetFD());
		return 0;
	}
	Stream* pUserStream = pStream;
	int isEnd = TRUE;
	if(m_pHttpResponse->GetState() == HEADER_NOTFOUND)
	{
		m_pStream->Append(pStream->GetData(),pStream->GetLength());
		m_bCloseClient = FALSE;
		int iHeaderSize = 0;
		if(iHeaderSize = m_pHttpResponse->IsHeaderEnd())
		{
			m_pHttpResponse->SetState(HEADER_FOUND);
			m_pHttpResponse->LoadHttpHeader();
			char* pConnection = m_pHttpResponse->GetHeader()->GetField(HTTP_CONNECTION);
			if(pConnection)
			{
				if(strstr(pConnection,"close") && m_pHttpResponse->GetHeader()->GetField(HTTP_CONTENT_LENGTH) == NULL && m_pHttpResponse->GetHeader()->GetField(HTTP_TRANSFER_ENCODING) == NULL)
				{
					m_bCloseClient = TRUE;
				}
			}

			if(m_pHttpResponse->HasBody())
			{
				m_pHttpResponse->LoadBody();
				Stream* pBodyStream = m_pStream->GetPartStream(iHeaderSize,m_pStream->GetLength());
				isEnd = m_pHttpResponse->GetBody()->IsEnd(pBodyStream);
			}
		}
	}
	else
	{
		if(m_pHttpResponse->HasBody())
			if(m_pHttpResponse->GetBody())
			isEnd = m_pHttpResponse->GetBody()->IsEnd(pUserStream);
	}
	if(m_pClientSide->GetSendStream()->GetLength()>0)
	{
		m_pClientSide->LockSendBuffer();
		m_pClientSide->GetSendStream()->Append(pUserStream->GetData(),pUserStream->GetLength());
		m_pClientSide->UnlockSendBuffer();
	}
	else
	{
		int flag = TRUE;
		while(flag)
		{
			m_pClientSide->LockSendBuffer();
			int nSent = send(m_pClientSide->GetEvent()->GetFD(),pUserStream->GetData(),pUserStream->GetLength(),0);
			if(nSent == -1)
			{
				if(errno == EAGAIN)
				{
					m_pClientSide->GetSendStream()->Append(pUserStream->GetData(),pUserStream->GetLength());
					m_pClientSide->UnlockSendBuffer();
				}
				else
				{
					GetEvent()->RemoveFromEngine();
					close(GetEvent()->GetFD());
				}

				flag = FALSE;
			}
			/*else if(nSent == 0)
			{
				GetEvent()->RemoveFromEngine();
				close(GetEvent()->GetFD());
				return 0;
			}*/
			else
			{
				pUserStream->Sub(nSent);
				if(pUserStream->GetLength() == 0)
				{
					SetCanRead(TRUE);
					m_pClientSide->SetCanWrite(FALSE);
					if(isEnd)
					{
						delete m_pHttpResponse;
						m_pHttpResponse = new HttpResponse(m_pStream);
						m_pStream->Sub(m_pStream->GetLength());
						m_iState = STATUS_IDLE;
						m_pClientSide->SetTransIdleState();
						SetCanRead(TRUE);
						m_pClientSide->SetCanWrite(FALSE);
						//m_pClientSide->GetEvent()->ModEvent(EPOLLIN|EPOLLERR|EPOLLET|EPOLLRDHUP);
					}
					m_pClientSide->UnlockSendBuffer();
					if(GetEvent()->IsInReady())
					{
						GetMasterThread()->InsertTask(GetRecvTask());
					}
					else
					{
						SetCanRead(TRUE);
					}
					flag = FALSE;
				}
			}
		}
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
	if(m_bCloseClient)
	{
		m_pClientSide->GetEvent()->RemoveFromEngine();
		close(m_pClientSide->GetEvent()->GetFD());
	}
	return 0;
}
