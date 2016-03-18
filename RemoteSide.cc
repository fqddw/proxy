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

int RemoteSide::SetStatusBlocking()
{
	m_iState = STATUS_BLOCKING; 
	return TRUE;
}

InetSocketAddress* RemoteSide::GetAddr()
{
	return m_pAddr;
}
RemoteSide::RemoteSide():IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream)
{
	GetEvent()->SetIOHandler(this);
}
RemoteSide::RemoteSide(InetSocketAddress* pAddr):IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream)
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
	if(m_pSendStream->GetLength()>0)
	{
		int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
		if(nSent > 0)
			m_pSendStream->Sub(nSent);
	}
		return 0;
		int totalSend = 0;
		int flag = TRUE;
		while(flag)
		{
				int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
				if(nSent == -1)
				{
						flag = FALSE;
				}
				else
				{
						totalSend += nSent;
				}
		}
		m_pSendStream->Sub(totalSend);
	return TRUE;
}

int RemoteSide::ProccessConnectionReset()
{
	return TRUE;
}
int RemoteSide::ProccessReceive(Stream* pStream)
{
	m_pStream->Append(pStream->GetData(),pStream->GetLength());
	if(m_pHttpResponse->GetState() == HEADER_NOTFOUND)
	{
		m_pHttpResponse->SetState(HEADER_FOUND);
		if(m_pHttpResponse->IsHeaderEnd())
		{
			m_pHttpResponse->LoadHttpHeader();
			if(m_pHttpResponse->HasBody())
			{
			}
		}
	}
	int ret=send(m_pClientSide->GetEvent()->GetFD(),pStream->GetData(),pStream->GetLength(),0);
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
