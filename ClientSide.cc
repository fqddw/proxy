#include "ClientSide.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
#include "MemList.h"
#include "netdb.h"
#include "InetSocketAddress.h"
#include "NetUtils.h"
#include "arpa/inet.h"
extern MemList<void*>* pGlobalList;
#define SEND_BUFFER_LENGTH 256*1024
ClientSide::ClientSide():IOHandler(),m_pStream(new Stream()),m_pSendStream(new Stream()),m_iSendEndPos(0),m_iAvaibleDataSize(0)
{
	GetEvent()->SetIOHandler(this);
	m_iState = HEADER_NOTFOUND;
	m_iTransState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
	delete m_pSendStream;
	if(m_pHttpRequest)
	{
		delete m_pHttpRequest;
		m_pHttpRequest = NULL;
	}
}
ClientSide::ClientSide(int sockfd):IOHandler(),m_pStream(new Stream()),m_pSendStream(new Stream())
{
	m_iTransState = CLIENT_STATE_IDLE;
	m_iState = HEADER_NOTFOUND;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
	m_pHttpRequest = new HttpRequest(m_pStream);
}

int ClientSide::Proccess()
{
		return FALSE;
}
int ClientSide::ClearHttpEnd()
{
	delete m_pHttpRequest;
	m_pHttpRequest = new HttpRequest(m_pStream);
	m_pStream->Sub(m_pStream->GetLength());
	m_pSendStream->Sub(m_pSendStream->GetLength());
	m_iState = HEADER_NOTFOUND;
	//SetCanRead(TRUE);
	//m_pClientSide->SetCanWrite(FALSE);
	return 0;

}
#include "stdlib.h"
int ClientSide::ProccessReceive(Stream* pStream)
{
	if(!pStream)
	{
		GetEvent()->CancelInReady();
		SetCanRead(TRUE);
		return 0;
	}


		if(m_iState == HEADER_NOTFOUND && m_iTransState != CLIENT_STATE_WAITING)
		{
			if(m_iTransState != CLIENT_STATE_RUNNING)
				m_iTransState = CLIENT_STATE_RUNNING;
			m_pStream->Append(pStream->GetData(),pStream->GetLength());
			if(m_pHttpRequest->IsHeaderEnd())
			{
				m_pHttpRequest->LoadHttpHeader();
				m_pHttpRequest->GetAuthStatus();
				m_iState = HEADER_FOUND;
				InetSocketAddress* pAddr = NULL;
				pAddr = NetUtils::GetHostByName(m_pHttpRequest->GetHeader()->GetUrl()->GetHost(),m_pHttpRequest->GetHeader()->GetUrl()->GetPort());
				if(!pAddr)
				{
					char* pText = (char*)"HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
					send(GetEvent()->GetFD(),pText,strlen(pText),0);
					close(GetEvent()->GetFD());
					return 0;
				}
				RemoteSide* pRemoteSide = GetRemoteSide(pAddr);
				m_pRemoteSide = pRemoteSide;
				Stream* pSendStream = m_pHttpRequest->GetHeader()->ToHeader();
				pRemoteSide->GetSendStream()->Append(pSendStream->GetData(),pSendStream->GetLength());
				delete pSendStream;
				SetCanWrite(TRUE);
				//GetEvent()->ModEvent(EPOLLOUT|EPOLLET);

				int hasBody = m_pHttpRequest->HasBody();
				if(!hasBody)
				{
					delete m_pHttpRequest;
					m_pHttpRequest = new HttpRequest(m_pStream);
					m_iState = HEADER_NOTFOUND;
					m_iTransState = CLIENT_STATE_WAITING;
				}
				else
				{
					m_pHttpRequest->LoadBody();
					Stream* pBodyStream = m_pStream->GetPartStream(m_pHttpRequest->GetHeader()->GetRawLength(),m_pStream->GetLength());
					if(m_pHttpRequest->GetBody()->IsEnd(pBodyStream))
					{
						m_iState = HEADER_NOTFOUND;
						m_iTransState = CLIENT_STATE_WAITING;
					}

					pRemoteSide->GetSendStream()->Append(pBodyStream->GetData(),pBodyStream->GetLength());
					delete pBodyStream;
					SetCanWrite(TRUE);
					m_pRemoteSide->SetCanWrite(TRUE);
					//注册写事件，接下来此链接将处理Client::ProccessSend,下一步设置在ProccessSend玩之后注册写事件
					GetEvent()->ModEvent(EPOLLOUT|EPOLLET);
					m_pRemoteSide->GetEvent()->ModEvent(EPOLLOUT|EPOLLET);

				}
				if(pRemoteSide->IsConnected())
				{
					pRemoteSide->SetCanWrite(TRUE);
					//GetMasterThread()->InsertTask(m_pRemoteSide->GetSendTask());
					pRemoteSide->ProccessSend();
				}
				m_pStream->Sub(m_pStream->GetLength());
			}
			else
			{
							SetCanRead(TRUE);
				return FALSE;
			}
		}
		else if(m_iState == HEADER_FOUND && m_iTransState == CLIENT_STATE_RUNNING)
		{
						if(!m_pRemoteSide)
						{
										printf("NULL Remote Body\n");
						}
			if(m_pHttpRequest->GetBody())
				if(m_pHttpRequest->GetBody()->IsEnd(pStream))
				{
						m_iState = HEADER_NOTFOUND;
						m_iTransState = CLIENT_STATE_WAITING;
				}
				m_pRemoteSide->GetSendStream()->Append(pStream->GetData(),pStream->GetLength());
				m_pRemoteSide->ProccessSend();
				m_pStream->Sub(m_pStream->GetLength());
				SetCanRead(TRUE);
		}
		else if(m_iTransState == CLIENT_STATE_WAITING)
		{
			close(m_pRemoteSide->GetEvent()->GetFD());
			m_pRemoteSide->SetClientSide(NULL);
		}

		return FALSE;
}

extern MemList<RemoteSide*>* g_pGlobalRemoteSidePool;

RemoteSide* ClientSide::GetRemoteSide(int fd)
{
	//g_pGlobalRemoteSidePool->Lock();
	RemoteSide* pRemoteSide=NULL;
	MemNode<RemoteSide*>* pSocketPool = g_pGlobalRemoteSidePool->GetHead();
	for(;pSocketPool!=NULL;pSocketPool = pSocketPool->GetNext())
	{
		RemoteSide* pSide = pSocketPool->GetData();
		if(pSide->GetEvent()->GetFD() == fd)
		{
			break;
		}
	}
}
RemoteSide* ClientSide::GetRemoteSide(InetSocketAddress* pAddr)
{
	//g_pGlobalRemoteSidePool->Lock();
	RemoteSide* pRemoteSide=NULL;
	MemNode<RemoteSide*>* pSocketPool = g_pGlobalRemoteSidePool->GetHead();
	for(;pSocketPool!=NULL;pSocketPool = pSocketPool->GetNext())
	{
		RemoteSide* pSide = pSocketPool->GetData();
		if(pSide->GetAddr()->Equal(pAddr) && pSide->IsIdle())
		{
			pSide->SetStatusBlocking();
			pRemoteSide = pSide;
			pRemoteSide->SetClientSide(this);
			break;
		}
	}

	if(!pRemoteSide)
	{
		pRemoteSide = new RemoteSide(pAddr);
		pRemoteSide->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pRemoteSide->SetMasterThread(GetMasterThread());
		pRemoteSide->SetClientSide(this);
		pRemoteSide->GetEvent()->AddToEngine(EPOLLOUT|EPOLLERR|EPOLLET|EPOLLRDHUP);
		g_pGlobalRemoteSidePool->Append(pRemoteSide);
	}
	//g_pGlobalRemoteSidePool->Unlock();
	return pRemoteSide;
}

Stream* ClientSide::GetSendStream(){
	return m_pSendStream;
}


int ClientSide::ProccessSend()
{
	//printf("Send Pending Length: %d\n",m_pSendStream->GetLength());
	if(m_pSendStream->GetLength()<=0)
	{
		//SetCanWrite(FALSE);
		return FALSE;
	}
	GetEvent()->CancelOutReady();
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
								printf("Client Send EAGAIN\n");
					if(GetEvent()->IsOutReady())
					{
								printf("Client Send EAGAIN Ready!\n");
						GetMasterThread()->InsertTask(GetSendTask());
						UnlockSendBuffer();
						return TRUE;
					}
					else
								SetCanWrite(TRUE);
				}
				else
				{
					printf("SIG PIPE SEND %d\n", m_pSendStream->GetLength());
					ClearHttpEnd();
					printf("SIG PIPE SEND AFTER %d\n", m_pSendStream->GetLength());
					m_pRemoteSide->SetClientSide(NULL);
					m_pRemoteSide->GetEvent()->RemoveFromEngine();
					close(m_pRemoteSide->GetEvent()->GetFD());
					g_pGlobalRemoteSidePool->Delete(m_pRemoteSide);
					//delete m_pRemoteSide;
					int sockfd = GetEvent()->GetFD();
					GetEvent()->RemoveFromEngine();
					if(pGlobalList->Delete(this))
					{
					}
					close(sockfd);
				}
			}
			else if(nSent == 0)
			{
					printf("SIG CLOSE WILL TRIGGER %d\n", m_pSendStream->GetLength());
					ClearHttpEnd();
				m_pRemoteSide->SetClientSide(NULL);
				m_pRemoteSide->SetStatusIdle();
				m_pRemoteSide->GetEvent()->RemoveFromEngine();
				close(m_pRemoteSide->GetEvent()->GetFD());
				int sockfd = GetEvent()->GetFD();
				GetEvent()->RemoveFromEngine();
				if(pGlobalList->Delete(this))
				{
				}
				close(sockfd);
			}
			else
			{
				totalSend += nSent;
				m_pSendStream->Sub(nSent);
				if(m_pSendStream->GetLength() == 0)
				{
					if(m_pRemoteSide->GetResponse()->GetBody())
					{
						if(m_pRemoteSide->GetResponse()->GetBody()->IsEnd())
						{
										printf("End Here Already\n");
										//此时pin链接已完成一条请求，重置各个事件状态，Client注册读事件，Remote注册写事件
							m_pRemoteSide->ClearHttpEnd();
							m_pRemoteSide->SetClientSide(NULL);
							m_pRemoteSide->SetCanRead(FALSE);
							m_pRemoteSide->SetCanWrite(TRUE);
							ClearHttpEnd();
							SetCanRead(TRUE);
							SetCanWrite(FALSE);
							GetEvent()->ModEvent(EPOLLIN|EPOLLET);

							m_pRemoteSide->GetEvent()->ModEvent(EPOLLOUT|EPOLLET);
							m_iTransState = CLIENT_STATE_IDLE;
							m_pRemoteSide->SetStatusIdle();
							//GetEvent()->ModEvent(EPOLLIN|EPOLLERR|EPOLLET|EPOLLRDHUP);
						}
						else
						{
										//请求正在传输中,engine此时屏蔽了remote的数据到达处理函数，但会设置是否有数据到达,如果有数据到达则投递处理任务，没有则开启处理函数
										if(m_pRemoteSide->GetEvent()->IsInReady())
										{
														printf("pull from remote\n");
														SetCanWrite(FALSE);
														m_pRemoteSide->SetCanRead(FALSE);
														m_pRemoteSide->GetEvent()->CancelInReady();
														GetMasterThread()->InsertTask(m_pRemoteSide->GetRecvTask());
										}
										else
										{
														SetCanWrite(FALSE);
														m_pRemoteSide->SetCanRead(TRUE);
										}
						}
					}
					else
					{
									printf("ClientSide>>ProccessSend>>NoBody>>ClearHttpEnd\n");
									m_pRemoteSide->ClearHttpEnd();
							m_pRemoteSide->SetClientSide(NULL);
							m_pRemoteSide->SetCanRead(FALSE);
							m_pRemoteSide->SetCanWrite(TRUE);
							ClearHttpEnd();
							SetCanRead(TRUE);
							SetCanWrite(FALSE);
							GetEvent()->ModEvent(EPOLLIN|EPOLLET);

							m_pRemoteSide->GetEvent()->ModEvent(EPOLLOUT|EPOLLET);
							m_iTransState = CLIENT_STATE_IDLE;
							m_pRemoteSide = NULL;
							return 0;

						//SetCanWrite(FALSE);
					}
					//SetCanWrite(TRUE);
					if(m_pRemoteSide->GetEvent()->IsInReady())
					{
						m_pRemoteSide->GetEvent()->CancelInReady();
						UnlockSendBuffer();
						GetMasterThread()->InsertTask(m_pRemoteSide->GetRecvTask());
						return TRUE;
					}
					else
					{
						m_pRemoteSide->SetCanRead(TRUE);
					}
					flag = FALSE;
				}
				else
				{
								printf("during transfer\n");
				}
			}
		}
	return FALSE;

}
void ClientSide::SetTransIdleState()
{
	m_iTransState = CLIENT_STATE_IDLE;
}
int ClientSide::ProccessConnectionReset()
{
	if(m_pRemoteSide)
	{
		//m_pRemoteSide->SetClientSide(NULL);
	}
}

HttpRequest* ClientSide::GetRequest()
{
	return m_pHttpRequest;
}

int ClientSide::GetSide()
{
	return CLIENT_SIDE;
}

int ClientSide::GetSendEndPos()
{
				return m_iSendEndPos;
}
int ClientSide::CanAppend(int nSize)
{
				if(m_iSendEndPos + nSize <= SEND_BUFFER_LENGTH)
				{
								return TRUE;
				}
				return FALSE;
}
int ClientSide::AppendSendStream(char* pData, int nSize)
{
				if(CanAppend(nSize))
				{
				}
}
