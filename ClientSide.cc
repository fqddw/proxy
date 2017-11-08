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
#include "mysql/mysql.h"
#include "AuthManager.h"
#include "QueuedNetTask.h"
extern MemList<void*>* pGlobalList;
#define SEND_BUFFER_LENGTH 256*1024
ClientSide::ClientSide():
	IOHandler(),
	m_pStream(new Stream()),
	m_pSendStream(new Stream()),
	m_iSendEndPos(0),
	m_iAvaibleDataSize(0),
	m_bCloseAsLength(FALSE),
	m_iRemoteState(STATE_NORMAL),
	m_bSSL(FALSE),
	m_iSSLState(SSL_START)
{
	m_iSide = CLIENT_SIDE;
	GetEvent()->SetIOHandler(this);
	m_iState = HEADER_NOTFOUND;
	m_iTransState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
	m_pStream = NULL;
	delete m_pSendStream;
	m_pSendStream = NULL;
	if(m_pHttpRequest)
	{
		delete m_pHttpRequest;
		m_pHttpRequest = NULL;
	}
}
ClientSide::ClientSide(int sockfd):
	IOHandler(),
	m_pStream(new Stream()),
	m_pSendStream(new Stream()),
	m_bCloseAsLength(FALSE),
	m_iRemoteState(STATE_NORMAL),
	m_bSSL(FALSE),
	m_iSSLState(SSL_START)
{
	m_iSide = CLIENT_SIDE;
	m_iTransState = CLIENT_STATE_IDLE;
	m_iState = HEADER_NOTFOUND;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
	m_pHttpRequest = new HttpRequest(m_pStream);
	SetMainTask(new QueuedNetTask());
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
	/*if(IsClosed())
	{
		ProccessConnectionReset();
		return FALSE;
	}*/
	return 0;
}
#include "stdlib.h"
int ClientSide::SSLTransferRecv(Stream* pStream)
{
	if(m_iRemoteState != STATE_RUNNING)
	{
		ProccessConnectionReset();
		return 0;
	}
	int iLength = m_pRemoteSide->GetSendStream()->GetLength();
	m_pRemoteSide->GetSendStream()->Append(pStream->GetData(), pStream->GetLength());
	//if(iLength == 0/*GetSendRefCount() == 0*/)
	//{
		//GetMasterThread()->InsertTask(m_pRemoteSide->GetSendTask());
	//}
	//else
	//{
	//	printf("Remote Sending In Proccess\n");
	//}
	m_pRemoteSide->SetSendFlag();
	LockTask();
	if(!GetMainTask()->IsRunning())
		GetMasterThread()->InsertTask(GetMainTask());
	UnlockTask();
	delete pStream;
	//SetCanRead(TRUE);
	/*if(IsClosed())
	{
		ProccessConnectionReset();
		//printf("Stalled\n");
	}*/
	return TRUE;
}

int ClientSide::SSLTransferCreate()
{
	SetCanRead(TRUE);
	m_bSSL = TRUE;
	InetSocketAddress* pAddr = NetUtils::GetHostByName(GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetHost(),GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetPort());

	if(!pAddr)
		exit(0);
	m_iSSLState = SSL_REMOTE_CONNECTING;
	RemoteSide* pRemoteSide = new RemoteSide(pAddr);
	m_pRemoteSide = pRemoteSide;
	pRemoteSide->EnableSSL();
	pRemoteSide->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
	pRemoteSide->SetMasterThread(GetMasterThread());
	pRemoteSide->SetCanWrite(TRUE);
	pRemoteSide->SetCanRead(FALSE);

	pRemoteSide->SetClientSide(this);
	pRemoteSide->SetClientState(STATE_RUNNING);
	pRemoteSide->SetMainTask(GetMainTask());
	m_iRemoteState = STATE_RUNNING;
	//printf("Create Connection SSL %s %d\n", GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetHost(), GetEvent()->GetFD());
	pRemoteSide->GetEvent()->AddToEngine(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
	pRemoteSide->SetSendFlag();
	LockTask();
	if(!GetMainTask()->IsRunning())
		GetMasterThread()->InsertTask(GetMainTask());
	UnlockTask();
	return TRUE;
}
#include "Digest.h"
#define REALM_STRING (char*)"www.transit-server.com"
int ClientSide::ProccessReceive(Stream* pStream)
{
	/*if(IsClosed() && m_bSSL)
	{
		if(pStream)
			delete pStream;
		ProccessConnectionReset();
		return FALSE;
	}*/
	if(!pStream)
	{
	/*	if(IsClosed())
		{
			//printf("Client Close %s %d\n", GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetHost(), m_pSendStream->GetLength());
			if(m_iRemoteState==STATE_RUNNING)
			{
				m_pRemoteSide->SetClosed(TRUE);
				//GetMasterThread()->InsertTask(m_pRemoteSide->GetRecvTask());
			}
			ProccessConnectionReset();
			return FALSE;
		}
		GetEvent()->CancelInReady();
		SetCanRead(TRUE);
		*/
		ProccessConnectionReset();
		return 0;
	}
	if(m_bSSL)
	{
		SSLTransferRecv(pStream);
		return TRUE;
	}


	if(m_iState == HEADER_NOTFOUND && m_iTransState != CLIENT_STATE_WAITING)
	{
		if(m_iTransState != CLIENT_STATE_RUNNING)
			m_iTransState = CLIENT_STATE_RUNNING;
		m_pStream->Append(pStream->GetData(),pStream->GetLength());
		if(m_pHttpRequest->IsHeaderEnd())
		{
			m_pHttpRequest->LoadHttpHeader();
			const char* phost = m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost();
			//if(strstr(m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost(), "www.iqiyi.com"))
			if(m_pHttpRequest->GetHeader()->GetField(HTTP_COOKIE))
			{
				MYSQL conn;
				MYSQL* h;
				mysql_init(&conn);
				h = &conn;
				mysql_real_connect(h, "localhost", "root", "123456", "ts", 0, NULL, 0);
				mysql_query(h, "SET NAMES utf8");
					//mysql_query(h, (string("INSERT INTO `user_session` SET `user_id`=1, `create_time`='0', `url`='")+string(phost)+string("',`session_key`='")+string(m_pHttpRequest->GetHeader()->GetField(HTTP_COOKIE))+string("'")).c_str());
				mysql_query(h, string(string("SELECT `session_key` FROM `user_session` WHERE `url`='")+string(phost)+string("'")).c_str());
				MYSQL_RES* res = mysql_use_result(h);
				MYSQL_ROW row = mysql_fetch_row(res);

				if(row)
				{
					m_pHttpRequest->GetHeader()->DeleteField((char*)"Cookie");
					m_pHttpRequest->GetHeader()->AppendHeader((char*)"Cookie", 6, row[0], strlen(row[0]));
				}
				mysql_free_result(res);
				mysql_close(h);
			}
			
			char* pAuthString = m_pHttpRequest->GetHeader()->GetField(HTTP_PROXY_AUTHENTICATION);
			if(pAuthString)
			{
				Stream* pAuthStream = new Stream();
				pAuthStream->Append(pAuthString, strlen(pAuthString));
				Digest* pDigest = new Digest(pAuthStream);
				pDigest->Parse();
				if(pDigest->GetRealm())
				{
				if(!pDigest->GetRealm()->Equal(REALM_STRING))
				{
					printf("error realm\n");
				}
				}
				else
				{
					printf("Not Realm\n");
				}
				pDigest->SetMethod(m_pHttpRequest->GetHeader()->GetRequestLine()->GetMethodStream());
				Stream* pRespStream = pDigest->CalcResponse();
				char* pData = pRespStream->GetData();
				if(strncmp(pData, pDigest->GetResponse()->GetData(), 32))
				{
					delete pRespStream;
					delete pDigest;
					const char* pAuthFailedText = "HTTP/1.1 200 OK\r\nServer: Turbo Load\r\nContent-Length: 10\r\n\r\nAuth Failed";
					send(GetEvent()->GetFD(), pAuthFailedText, strlen(pAuthFailedText), 0);
					ProccessConnectionReset();
					return 0;
				}
				delete pRespStream;
				delete pDigest;

				//printf("%s\n", pDigest->CalcH1()->GetData());
			}
			//int authResult = m_pHttpRequest->GetAuthStatus();
			//if(authResult)
			else
			{
				Auth* pAuth = AuthManager::getInstance()->GenerateAuthToken();

				pAuth->SetRealm(REALM_STRING);
				HttpResponseHeader* pAuthResp = new HttpResponseHeader();
				pAuthResp->GetResponseLine()->SetCode(407);
				pAuthResp->GetResponseLine()->SetMajorVersion(1);
				pAuthResp->GetResponseLine()->SetSeniorVersion(1);
				const char* chText = "Proxy Authentication Required";
				int statusTextLen = strlen(chText);
				char* pStatusText = new char[statusTextLen+1];
				pStatusText[statusTextLen] = '\0';
				memcpy(pStatusText, chText, statusTextLen);
				pAuthResp->GetResponseLine()->SetStatusText(pStatusText);
				pAuthResp->SetKeyValueList(new HttpKeyValueList());
				Stream* pAuthStream = pAuth->ToStream();
				pAuthResp->AppendHeader((char*)"Proxy-Authenticate", 18, pAuthStream->GetData(), pAuthStream->GetLength());
				pAuthResp->AppendHeader((char*)"Content-Length", 14, (char*)"0", 1);
				Stream* pAuthRespStream = pAuthResp->ToHeader();
				//printf("%s", pAuthRespStream->GetData());
				send(GetEvent()->GetFD(), pAuthRespStream->GetData() , pAuthRespStream->GetLength(), 0);
				delete pAuthRespStream;
				delete pAuthResp;
				delete pAuth;
				delete pAuthString;
				ClearHttpEnd();
				m_iState = HEADER_NOTFOUND;
				m_iTransState = CLIENT_STATE_IDLE;
				GetEvent()->ModEvent(EPOLLIN|EPOLLONESHOT);
				return FALSE;
			}
			m_iState = HEADER_FOUND;
			InetSocketAddress* pAddr = NULL;
			//if(strstr(m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost(), "p.l.youku.com"))
					//return 0;
			pAddr = NetUtils::GetHostByName(m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost(),m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetPort());
			if(!pAddr)
			{
				char* pText = (char*)"HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
				send(GetEvent()->GetFD(),pText,strlen(pText),0);
				ProccessConnectionReset();
				return 0;
			}
			if(m_pHttpRequest->GetHeader()->GetRequestLine()->GetMethod() == HTTP_METHOD_CONNECT)
			{
				//printf("%s", m_pStream->GetData());
				delete pAddr;
				SetCanRead(TRUE);
				SSLTransferCreate();
				//ProccessConnectionReset();
				return FALSE;
			}
			else
			{
				//printf("Url %s %s\n", m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost(), m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->ToString());
			}

			RemoteSide* pRemoteSide = GetRemoteSide(pAddr);
			m_pRemoteSide = pRemoteSide;
			Stream* pSendStream = m_pHttpRequest->GetHeader()->ToHeader();
			pRemoteSide->GetSendStream()->Append(pSendStream->GetData(),pSendStream->GetLength());
			delete pSendStream;
			//SetCanWrite(TRUE);
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
				if(pBodyStream)
				{
					if(m_pHttpRequest->GetBody()->IsEnd(pBodyStream))
					{
						m_iState = HEADER_NOTFOUND;
						m_iTransState = CLIENT_STATE_WAITING;
					}

					pRemoteSide->GetSendStream()->Append(pBodyStream->GetData(),pBodyStream->GetLength());
					delete pBodyStream;
				}

			}
			pRemoteSide->SetCanRead(FALSE);
			pRemoteSide->SetCanWrite(TRUE);
			/*if(pRemoteSide->IsConnected())
				pRemoteSide->GetEvent()->ModEvent(EPOLLOUT|EPOLLET|EPOLLONESHOT);
			else*/
				//GetMasterThread()->InsertTask(pRemoteSide->GetSendTask());
			{
				pRemoteSide->SetSendFlag();
				LockTask();
				if(GetMainTask()->IsRunning())
				{
				}
				else
				{
					GetMasterThread()->InsertTask(GetMainTask());
				}
				UnlockTask();
			}
			/*if(pRemoteSide->IsConnected())
			  {
			  pRemoteSide->ProccessSend();
			  }*/
			m_pStream->Sub(m_pStream->GetLength());
			SetCanRead(TRUE);
		}
		else
		{
			GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
			SetCanRead(TRUE);
			return FALSE;
		}
	}
	else if(m_iState == HEADER_FOUND && m_iTransState == CLIENT_STATE_RUNNING)
	{
		if(!m_pRemoteSide)
		{
		}
		if(m_pHttpRequest->GetBody())
			if(m_pHttpRequest->GetBody()->IsEnd(pStream))
			{
				m_iState = HEADER_NOTFOUND;
				m_iTransState = CLIENT_STATE_WAITING;
			}
		int nLength = m_pRemoteSide->GetSendStream()->GetLength();
		m_pRemoteSide->GetSendStream()->Append(pStream->GetData(),pStream->GetLength());
		/*if(nLength == 0)
		{

			//printf("Multi Thread RecvTask %s %d\n", __FILE__, __LINE__);
			GetMasterThread()->InsertTask(m_pRemoteSide->GetSendTask());
		}*/
		m_pRemoteSide->SetSendFlag();
		LockTask();
		if(!GetMainTask()->IsRunning())
			GetMasterThread()->InsertTask(GetMainTask());
		UnlockTask();
		//m_pRemoteSide->ProccessSend();
		m_pStream->Sub(m_pStream->GetLength());
		SetCanRead(TRUE);
	}
	else if(m_iTransState == CLIENT_STATE_WAITING)
	{
		//printf("logic error!\n");
		ProccessConnectionReset();
		return FALSE;
	}

	/*if(IsClosed())
	{
		ProccessConnectionReset();
		return FALSE;
	}*/

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
	return NULL;
}
RemoteSide* ClientSide::GetRemoteSide(InetSocketAddress* pAddr)
{
	RemoteSide* pRemoteSide=NULL;
	g_pGlobalRemoteSidePool->Lock();
	MemNode<RemoteSide*>* pSocketPool = g_pGlobalRemoteSidePool->GetHead();
	for(;pSocketPool!=NULL;pSocketPool = pSocketPool->GetNext())
	{
		RemoteSide* pSide = pSocketPool->GetData();
		if(pSide->GetAddr()->Equal(pAddr) && pSide->IsIdle())
		{
			pSide->SetStatusBlocking();
			pRemoteSide = pSide;
			pRemoteSide->SetCanWrite(FALSE);
			pRemoteSide->SetCanRead(TRUE);
			pRemoteSide->SetClientSide(this);
			pRemoteSide->SetClientState(STATE_RUNNING);
			pRemoteSide->SetMainTask(GetMainTask());

			m_iRemoteState = STATE_RUNNING;
			break;
		}
	}

	g_pGlobalRemoteSidePool->Unlock();
	if(!pRemoteSide)
	{
		pRemoteSide = new RemoteSide(pAddr);
		pRemoteSide->GetEvent()->SetNetEngine(GetEvent()->GetNetEngine());
		pRemoteSide->SetMasterThread(GetMasterThread());
		pRemoteSide->SetCanWrite(FALSE);
		pRemoteSide->SetCanRead(TRUE);

		pRemoteSide->SetClientSide(this);
		pRemoteSide->SetMainTask(GetMainTask());
		pRemoteSide->SetClientState(STATE_RUNNING);
		m_iRemoteState = STATE_RUNNING;
		pRemoteSide->GetEvent()->AddToEngine(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
		g_pGlobalRemoteSidePool->Append(pRemoteSide);
	}

	return pRemoteSide;
}

Stream* ClientSide::GetSendStream(){
	return m_pSendStream;
}


int ClientSide::ProccessSend()
{
	//远端退出
	if(m_iRemoteState == STATE_ABORT)
	{
		SetClosed(TRUE);
		//ProccessReceive(NULL);
		//ProccessConnectionReset();
		return TRUE;
	}
	//应该不可能出现这种情况
	if(m_pSendStream->GetLength()<=0)
	{
		//SetCanWrite(TRUE);
		return FALSE;
	}
	GetEvent()->CancelOutReady();
	if(GetEvent()->GetEventInt() & EPOLLOUT)
	{
		if(GetEvent()->GetEventInt() & EPOLLIN)
			SetCanWrite(FALSE);
		SetCanRead(TRUE);
		GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
	}
	else
	{
	}
	int totalSend = 0;
	int flag = TRUE;
	//printf("Client Send Data %s %d\n", GetRequest()->GetHeader()->GetRequestLine()->GetUrl()->GetHost(), m_pSendStream->GetLength());
	while(flag)
	{
		LockSendBuffer();
		if(IsClosed())
		{
			//printf("Send Close\n");
			return 0;
		}
		int nSent = send(GetEvent()->GetFD(),m_pSendStream->GetData(),m_pSendStream->GetLength(),0);
		//printf("Client Send %s\n", m_pSendStream->GetData());
		if(nSent < 0)
		{
			flag = FALSE;
			if(errno == EAGAIN)
			{
				if(0)//GetEvent()->IsOutReady())
				{
					GetEvent()->CancelOutReady();
					//printf("Multi Thread RecvTask %s %d\n", __FILE__, __LINE__);
					//GetMasterThread()->InsertTask(GetSendTask());
					UnlockSendBuffer();
					return TRUE;
				}
				else
				{
					//printf("EPOLLOUT TRIGGER\n");
					SetCanRead(FALSE);
					SetCanWrite(TRUE);
					GetEvent()->ModEvent(EPOLLOUT|/*EPOLLET|*/EPOLLONESHOT);
					return TRUE;
				}
			}
			else
			{
				m_pRemoteSide->SetClientState(STATE_ABORT);
				SetClosed(TRUE);
				//ProccessReceive(NULL);
				//ProccessConnectionReset();
				return 0;
			}
		}
		else if(nSent == 0)
		{
			printf("Client Send 0\n");
			//m_pRemoteSide->SetClientState(STATE_ABORT);
			SetClosed(TRUE);
			//ProccessReceive(NULL);
			//ProccessConnectionReset();
			return 0;
		}
		else
		{
			totalSend += nSent;
			m_pSendStream->Sub(nSent);
			if(m_pSendStream->GetLength() == 0)
			{
				//printf("Send Finish %d\n", GetEvent()->GetFD());
				flag = FALSE;
				if(m_iRemoteState == STATE_NORMAL)
				{
					//此时pin链接已完成一条请求，重置各个事件状态，Client注册读事件
					if(m_bCloseAsLength == TRUE)
					{
						SetClosed(TRUE);
						ProccessConnectionReset();
						return TRUE;
					}
					ClearHttpEnd();
					SetCanRead(TRUE);
					SetCanWrite(FALSE);
					m_iTransState = CLIENT_STATE_IDLE;
					/*if(m_bSSL)
						printf("Remote SSL Close\n");*/

					return 0;
				}
				else
				{
					m_pRemoteSide->GetEvent()->ModEvent(EPOLLIN|/*EPOLLET|*/EPOLLONESHOT);
					return 0;
					//请求正在传输中,engine此时屏蔽了remote的数据到达处理函数，但会设置是否有数据到达,如果有数据到达则投递处理任务，没有则开启处理函数
					if(GetEvent()->IsInReady())
					{
						//printf("May Lost Data Here\n");
					}
					if(m_pRemoteSide->GetEvent()->IsInReady())
					{
						SetCanWrite(FALSE);
						m_pRemoteSide->SetCanRead(FALSE);
						m_pRemoteSide->GetEvent()->CancelInReady();
						//GetMasterThread()->InsertTask(m_pRemoteSide->GetRecvTask());
					}
					else
					{
						SetCanWrite(FALSE);
						SetCanRead(TRUE);
						m_pRemoteSide->SetCanRead(TRUE);
					}
				}
				/*SetCanWrite(FALSE);
				  SetCanRead(TRUE);
				  GetEvent()->ModEvent(EPOLLIN|EPOLLET);*/
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
int ClientSide::ProccessConnectionClose()
{
	return ProccessConnectionReset();
}
int ClientSide::ProccessConnectionReset()
{
	//printf("%d %d\n", GetEvent()->GetFD(), GetRefCount());
	//printf("%d %d %d %d\n", GetEvent()->GetFD(), GetRefCount(), GetRecvRefCount(), GetSendRefCount());
	/*if(GetRefCount() > 2)
	{
		return 0;
	}*/
	if(IsRealClosed())
	{
		return 0;
	}
	SetRealClosed(TRUE);
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
					//m_pRemoteSide->ProccessConnectionReset();
					//printf("Multi Thread RecvTask %s %d\n", __FILE__, __LINE__);
					//GetMasterThread()->InsertTask(m_pRemoteSide->GetRecvTask());
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
	//pGlobalList->Delete(this);
	close(sockfd);
	m_pRemoteSide = NULL;
	GetMainTask()->SetClient(NULL);
	Release();
	return 0;
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
	return 0;
}

void ClientSide::SetRemoteState(int iState)
{
	m_iRemoteState = iState;
}

void ClientSide::SetCloseAsLength(int bCloseAsLength)
{
	m_bCloseAsLength = bCloseAsLength;
}

void ClientSide::SetRemoteSide(RemoteSide* pRemoteSide)
{
	m_pRemoteSide = pRemoteSide;
}

Stream* ClientSide::GetStream()
{
	return m_pStream;
}


void ClientSide::SetRecvFlag()
{
	GetMainTask()->SetClientRecving();
}


void ClientSide::SetSendFlag()
{
	GetMainTask()->SetClientSending();
}

void ClientSide::SetMainTask(QueuedNetTask* pTask)
{
	IOHandler::SetMainTask(pTask);
	if(pTask)
		pTask->SetClient(this);
}
