#ifndef __CLIENTSIDE_H__
#define __CLIENTSIDE_H__
#include "IOHandler.h"
#include "Stream.h"
#include "RemoteSide.h"
#include "HttpRequest.h"
#include "InetSocketAddress.h"
#define CLIENT_STATE_IDLE 1
#define CLIENT_STATE_RUNNING 2
#define CLIENT_STATE_WAITING 3
#define SSL_REMOTE_CONNECTING 1
#define SSL_START 0
class RemoteSide;
class ClientSide : public IOHandler
{
	public:
		ClientSide();
		virtual ~ClientSide();
		ClientSide(int);
		int Proccess();
		RemoteSide* GetRemoteSide(InetSocketAddress*);
		RemoteSide* GetRemoteSide(int);
		int ProccessReceive(Stream*);
		int ProccessConnectionReset();
		int ProccessSend();
		Stream* GetSendStream();
		int WriteData();
		void SetTransIdleState();
		HttpRequest* GetRequest();
		int ClearHttpEnd();
		int GetSide();
		int GetSendEndPos();
		int CanAppend(int);
		int AppendSendStream(char*, int);
		void SetRemoteState(int);
		void SetCloseAsLength(int);
		int SSLTransferRecv(Stream*);
		int SSLTransferCreate();
		void SetRemoteSide(RemoteSide*);
		Stream* GetStream();
	private:
		int m_iState;
		int m_iTransState;
		Stream* m_pStream;
		Stream* m_pSendStream;
		int m_iSendEndPos;
		int m_iAvaibleDataSize;
		HttpRequest* m_pHttpRequest;
		RemoteSide* m_pRemoteSide;
		int m_iRemoteState;
		int m_bCloseAsLength;
		int m_bSSL;
		int m_iSSLState;
};
#endif
