#include "RemoteSide.h"
#include "InetSocketAddress.h"
#include "fcntl.h"
#include "stdio.h"
#include "errno.h"
#include "NetUtils.h"
#include "unistd.h"
#include "string.h"

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

	m_iState = STATUS_IDLE; 
	SetCanRead(TRUE);
	SetCanWrite(FALSE);
	return TRUE;
}

InetSocketAddress* RemoteSide::GetAddr()
{
	return m_pAddr;
}
RemoteSide::RemoteSide():IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream),m_iState(STATUS_BLOCKING),m_isConnected(FALSE),m_iClientState(STATE_NORMAL),m_bCloseClient(FALSE)
{
				m_iSide = REMOTE_SIDE;
	GetEvent()->SetIOHandler(this);
}
RemoteSide::RemoteSide(InetSocketAddress* pAddr):IOHandler(),m_pSendStream(new Stream),m_pStream(new Stream),m_iState(STATUS_BLOCKING),m_isConnected(FALSE),m_iClientState(STATE_NORMAL),m_bCloseClient(FALSE)
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
	m_isConnected = SOCKTE_STATUS_CONNECTING;
	int ret = connect(m_iSocket,&sa,sizeof(sa));
	return ret;
}
int RemoteSide::ProccessSend()
{
				if(m_pSendStream->GetLength() == 0)
				{
								//SetCanWrite(TRUE);
								return FALSE;
				}

				/*if(!m_pClientSide)
								return FALSE;*/
				//处理连接操作
	if(m_isConnected == SOCKTE_STATUS_CONNECTING)
	{
					int error = 0;
					socklen_t size = sizeof(int);
					getsockopt(GetEvent()->GetFD(), SOL_SOCKET, SO_ERROR, (char*)&error, &size);
					if(error != 0)
					{
									SetClosed(FALSE);
									SetCanWrite(FALSE);
									GetEvent()->ModEvent(EPOLLIN|EPOLLET);
									SetClosed(TRUE);
									//ProccessConnectionReset();
									//m_pClientSide->ProccessConnectionReset();
									return 0;
					}
		if(m_pSendStream->GetLength())
						SetCanWrite(FALSE);
		m_isConnected = TRUE;
		//GetEvent()->ModEvent(EPOLLIN|EPOLLET);
	}
				if(!IsConnected())
				{
								Connect();
								SetCanWrite(TRUE);
								return FALSE;
				}
				if(GetEvent()->GetEventInt() & EPOLLOUT)
				{
								SetCanRead(TRUE);
								SetCanWrite(FALSE);
								GetEvent()->ModEvent(EPOLLIN|EPOLLET);
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
				{
								//重新启动可写触发的响应
								if(GetEvent()->IsOutReady())
								{
												GetEvent()->CancelOutReady();
												GetMasterThread()->InsertTask(GetSendTask());
												UnlockSendBuffer();
												return TRUE;
								}
								else
								{
												SetCanRead(FALSE);
												SetCanWrite(TRUE);
												GetEvent()->ModEvent(EPOLLOUT|EPOLLET);
								}
				}
				else
				{
								SetClosed(TRUE);
					return 0;
				}
			}else if(nSent == 0)
			{
							//ProccessConnectionReset();
				return 0;
			}
			else
			{
				totalSend += nSent;
				m_pSendStream->Sub(nSent);
				if(m_pSendStream->GetLength() == 0)
				{
								if(m_iClientState != STATE_ABORT)
								{
					if(m_pClientSide->GetRequest()->GetBody())
					{
						if(m_pClientSide->GetRequest()->GetBody()->IsEnd())
						{
							flag = FALSE;
							SetCanRead(TRUE);
							SetCanWrite(flag);
							//m_pClientSide->SetCanWrite(TRUE);
							//m_pClientSide->SetCanRead(FALSE);
							GetEvent()->ModEvent(EPOLLIN|EPOLLET);
							//m_pClientSide->GetEvent()->ModEvent(EPOLLET|EPOLLOUT);
						}
						else
						{
										if(m_pClientSide->GetEvent()->IsInReady())
										{
														SetCanWrite(FALSE);
														m_pClientSide->SetCanRead(FALSE);
														m_pClientSide->GetEvent()->CancelInReady();
														GetMasterThread()->InsertTask(m_pClientSide->GetRecvTask());
										}
										else
										{
														m_pClientSide->SetCanRead(TRUE);
										}

										flag = FALSE;
					}
					}
					else
					{
									//发送完成，注册读事件，使链接可以处理接受远程服务器数据
						flag = FALSE;
						SetCanRead(TRUE);
						SetCanWrite(flag);
						GetEvent()->ModEvent(EPOLLIN|EPOLLET);
						//m_pClientSide->SetCanWrite(TRUE);
						m_pClientSide->SetCanRead(FALSE);

						//m_pClientSide->GetEvent()->ModEvent(EPOLLET|EPOLLOUT);
					}
								}
								else
								{
												SetClosed(TRUE);
												//ProccessConnectionReset();
								}
				}
			}
			UnlockSendBuffer();
		}
	return TRUE;
}

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;
int RemoteSide::ClearHttpEnd()
{
	delete m_pHttpResponse;
	m_pHttpResponse = new HttpResponse(m_pStream);
	m_pStream->Sub(m_pStream->GetLength());
	m_iState = STATUS_IDLE;
	SetCanRead(TRUE);
	SetCanWrite(FALSE);
	if(m_pClientSide)
	{
					m_pClientSide->SetTransIdleState();
					m_pClientSide->SetCanWrite(FALSE);
	}
	return 0;

}
int RemoteSide::ProccessReceive(Stream* pStream)
{
	if(!pStream)
	{
					if(IsClosed())
					{
									ProccessConnectionClose();
					}
					else
									SetCanRead(TRUE);
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
	int isEnd = TRUE;
	if(m_pHttpResponse->GetState() == HEADER_NOTFOUND)
	{
		m_pStream->Append(pStream->GetData(),pStream->GetLength());
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
								m_pClientSide->SetCloseAsLength(TRUE);
				}
			}
			if(m_pHttpResponse->HasBody())
			{
				m_pHttpResponse->LoadBody();
				Stream* pBodyStream = m_pStream->GetPartStream(iHeaderSize,m_pStream->GetLength());
				isEnd = m_pHttpResponse->GetBody()->IsEnd(pBodyStream);
				delete pBodyStream;
			}
			m_pStream->Sub(m_pStream->GetLength());
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
									return 0;
					}

		int nLengthSend = m_pClientSide->GetSendStream()->GetLength();
		m_pClientSide->LockSendBuffer();
		m_pClientSide->GetSendStream()->Append(pUserStream->GetData(),pUserStream->GetLength());
		m_pClientSide->UnlockSendBuffer();
		ClientSide* pClientSide = m_pClientSide;
		if(isEnd)
		{
						//如果拉取信息结束,则解耦
						if(m_iClientState == STATE_RUNNING)
						{
										m_iClientState = STATE_NORMAL;
										m_pClientSide->SetRemoteState(STATE_NORMAL);
										m_pClientSide = NULL;

										ClearHttpEnd();
										SetStatusIdle();
						}
		}

						if(nLengthSend == 0)
						{
										//				SetCanRead(TRUE);
										GetMasterThread()->InsertTask(pClientSide->GetSendTask());
						}
						else
						{
						}
						if(IsClosed())
						{
										ProccessConnectionClose();
						}

		delete pUserStream;

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
				if(IsIdle())
				{
				}
				else
				{
								if(m_iClientState != STATE_ABORT)
								{
												m_pClientSide->SetRemoteState(STATE_ABORT);
												m_iClientState = STATE_NORMAL;
												ClearHttpEnd();
												GetMasterThread()->InsertTask(m_pClientSide->GetRecvTask());
								}
				}
	int sockfd = GetEvent()->GetFD();
	GetEvent()->RemoveFromEngine();
	g_pGlobalRemoteSidePool->Delete(this);
	close(sockfd);

	delete this;
	return 0;
}
int RemoteSide::ProccessConnectionClose()
{
				if(IsIdle())
				{
				}
				else
				{
								if(m_iClientState != STATE_ABORT)
								{
								m_pClientSide->SetRemoteState(STATE_NORMAL);
								m_iClientState = STATE_NORMAL;
								ClearHttpEnd();
								}
				}
	int sockfd = GetEvent()->GetFD();
	GetEvent()->RemoveFromEngine();
	g_pGlobalRemoteSidePool->Delete(this);
	close(sockfd);

	delete this;
	return 0;
}

int RemoteSide::GetSide()
{
	return REMOTE_SIDE;
}
RemoteSide::~RemoteSide()
{
				delete m_pStream;
				delete m_pSendStream;
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
