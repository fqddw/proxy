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
ClientSide::ClientSide():IOHandler(),m_pStream(new Stream()),m_pSendStream(new Stream()),m_iSendEndPos(0),m_iAvaibleDataSize(0),m_bCloseAsLength(FALSE)
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
ClientSide::ClientSide(int sockfd):IOHandler(),m_pStream(new Stream()),m_pSendStream(new Stream()),m_bCloseAsLength(FALSE)
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
				SetCanRead(TRUE);
				SetCanWrite(FALSE);
				//m_pClientSide->SetCanWrite(FALSE);
				return 0;

}
#include "stdlib.h"
int ClientSide::ProccessReceive(Stream* pStream)
{
				if(!pStream)
				{
								if(IsClosed())
								{
												ProccessConnectionReset();
												return FALSE;
								}
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
												if(m_pHttpRequest->GetHeader()->GetRequestLine()->GetMethod() == HTTP_METHOD_CONNECT)
												{
																ProccessConnectionReset();
																return FALSE;
												}
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

												}
												/*if(pRemoteSide->IsConnected())
												{
																pRemoteSide->ProccessSend();
												}*/
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
								int nLength = m_pRemoteSide->GetSendStream()->GetLength();
								m_pRemoteSide->GetSendStream()->Append(pStream->GetData(),pStream->GetLength());
								if(nLength == 0)
												m_pRemoteSide->ProccessSend();
								m_pStream->Sub(m_pStream->GetLength());
				}
				else if(m_iTransState == CLIENT_STATE_WAITING)
				{
								printf("logic error here %s %d\n", __FILE__, __LINE__);
								close(m_pRemoteSide->GetEvent()->GetFD());
								m_pRemoteSide->SetClientSide(NULL);
				}

				if(IsClosed())
				{
								ProccessConnectionReset();
								return FALSE;
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
												pRemoteSide->SetCanWrite(TRUE);
												pRemoteSide->SetCanRead(FALSE);
												pRemoteSide->SetClientSide(this);
												pRemoteSide->SetClientState(STATE_RUNNING);

												m_iRemoteState = STATE_RUNNING;
												pRemoteSide->GetEvent()->ModEvent(EPOLLOUT|EPOLLERR|EPOLLET|EPOLLRDHUP);
												break;
								}
				}

				if(!pRemoteSide)
				{
								pRemoteSide = new RemoteSide(pAddr);
								pRemoteSide->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
								pRemoteSide->SetMasterThread(GetMasterThread());
								pRemoteSide->SetCanWrite(TRUE);
								pRemoteSide->SetCanRead(FALSE);

								pRemoteSide->SetClientSide(this);
								pRemoteSide->SetClientState(STATE_RUNNING);
								m_iRemoteState = STATE_RUNNING;
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
				if(m_iRemoteState == STATE_ABORT)
				{
								ProccessConnectionReset();
								return TRUE;
				}
				if(m_pSendStream->GetLength()<=0)
				{
								SetCanWrite(TRUE);
								return FALSE;
				}
				GetEvent()->CancelOutReady();
				int totalSend = 0;
				int flag = TRUE;
				while(flag)
				{
								LockSendBuffer();
								int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
								if(nSent < 0)
								{
												flag = FALSE;
												if(errno == EAGAIN)
												{
																if(GetEvent()->IsOutReady())
																{
																				GetEvent()->CancelOutReady();
																				GetMasterThread()->InsertTask(GetSendTask());
																				UnlockSendBuffer();
																				return TRUE;
																}
																else
																				SetCanWrite(TRUE);
												}
												else
												{
																m_pRemoteSide->SetClientState(STATE_ABORT);
																ProccessConnectionReset();
																return 0;
												}
								}
								else if(nSent == 0)
								{
												m_pRemoteSide->SetClientState(STATE_ABORT);
												ProccessConnectionReset();
												return 0;
								}
								else
								{
												totalSend += nSent;
												m_pSendStream->Sub(nSent);
												if(m_pSendStream->GetLength() == 0)
												{
																flag = FALSE;
																if(m_iRemoteState == STATE_NORMAL)
																{
																				//此时pin链接已完成一条请求，重置各个事件状态，Client注册读事件
																				if(m_bCloseAsLength == TRUE)
																				{
																								ProccessConnectionReset();
																								return TRUE;
																				}
																				ClearHttpEnd();
																				SetCanRead(TRUE);
																				SetCanWrite(FALSE);
																				m_iTransState = CLIENT_STATE_IDLE;
																				GetEvent()->ModEvent(EPOLLIN|EPOLLET);

																				return 0;
																}
																else
																{
																				//请求正在传输中,engine此时屏蔽了remote的数据到达处理函数，但会设置是否有数据到达,如果有数据到达则投递处理任务，没有则开启处理函数
																				if(m_pRemoteSide->GetEvent()->IsInReady())
																				{
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
												}
												//SetCanWrite(TRUE);
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
				//如果远端正常关闭则代表远端已经自我清理
				///如果远端没有正常关闭，则远端已经自我清理完毕，并已经通知本地
				//逻辑本地与远端可互换
				if(m_iTransState == CLIENT_STATE_IDLE)
				{
				}
				else
				{
								switch(m_iRemoteState)
								{
												//正常结束为NORMAL
												case STATE_NORMAL:
																{
																}
																break;
																//正在传输，此时应该标记远端为ABORT
												case STATE_RUNNING:
																{
																				m_pRemoteSide->SetClientState(STATE_ABORT);
																				m_pRemoteSide->SetClientSide(NULL);
																				m_pRemoteSide = NULL;
																}
																break;
																//远端已经关闭且自我清理
												case STATE_ABORT:
																{
																				m_pRemoteSide = NULL;
																}
																break;
												default:
																;
								}
								ClearHttpEnd();
				}
				int sockfd = GetEvent()->GetFD();
				GetEvent()->RemoveFromEngine();
				pGlobalList->Delete(this);
				close(sockfd);
				m_pRemoteSide = NULL;
				delete this;
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

void ClientSide::SetRemoteState(int iState)
{
				m_iRemoteState = iState;
}

void ClientSide::SetCloseAsLength(int bCloseAsLength)
{
								m_bCloseAsLength = bCloseAsLength;
}
