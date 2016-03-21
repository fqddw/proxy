#include "RemoteSide.h"
#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
#include "errno.h"
#include "NetUtils.h"
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
	return TRUE;
}

InetSocketAddress* RemoteSide::GetAddr()
{
	return m_pAddr;
}
RemoteSide::RemoteSide():IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream),m_iState(STATUS_BLOCKING)
{
	GetEvent()->SetIOHandler(this);
}
RemoteSide::RemoteSide(InetSocketAddress* pAddr):IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream),m_iState(STATUS_BLOCKING)
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
	int ret = connect(m_iSocket,&sa,sizeof(sa));
	return ret;
}
int RemoteSide::ProccessSend()
{
		int totalSend = 0;
		int flag = TRUE;
		while(flag)
		{
				int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
				if(nSent == -1)
				{
					if(errno != EAGAIN)
						flag = FALSE;
				}
				else
				{
						totalSend += nSent;
						m_pSendStream->Sub(nSent);
						if(m_pSendStream->GetLength() == 0)
						{
							flag = FALSE;
						}
				}
		}
	return TRUE;
}

int RemoteSide::ProccessConnectionReset()
{
	return TRUE;
}
int RemoteSide::ProccessReceive(Stream* pStream)
{
	Stream* pUserStream = pStream;
	m_pStream->Append(pStream->GetData(),pStream->GetLength());
	int isEnd = FALSE;
	if(m_pHttpResponse->GetState() == HEADER_NOTFOUND)
	{
		m_pHttpResponse->SetState(HEADER_FOUND);
		int iHeaderSize = 0;
		if(iHeaderSize = m_pHttpResponse->IsHeaderEnd())
		{
			m_pHttpResponse->LoadHttpHeader();
			if(m_pHttpResponse->HasBody())
			{
				m_pHttpResponse->LoadBody();
				Stream* pBodyStream = pStream->GetPartStream(iHeaderSize,pStream->GetLength());
				isEnd = m_pHttpResponse->GetBody()->IsEnd(pBodyStream);
				printf("Header %d\n",isEnd);
			}
		}
	}
	else
	{
		isEnd = m_pHttpResponse->GetBody()->IsEnd(pUserStream);
	}
	int flag = TRUE;
	while(flag)
	{
		int nSent = send(m_pClientSide->GetEvent()->GetFD(),pUserStream->GetData(),pUserStream->GetLength(),0);
		if(nSent == -1)
		{
			if(errno == EAGAIN)
			{
				m_pClientSide->GetSendStream()->Append(pUserStream->GetData(),pUserStream->GetLength());
			}
			flag = FALSE;
		}
		else
		{
			pUserStream->Sub(nSent);
			if(pUserStream->GetLength() == 0)
			{
				if(isEnd)
				{
					delete m_pHttpResponse;
					m_pHttpResponse = new HttpResponse(m_pStream);
					m_iState = STATUS_IDLE;
				}
				flag = FALSE;
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
