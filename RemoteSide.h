#ifndef __REMOTESIDE_H__
#define __REMOTESIDE_H__

#include "CommonType.h"
#include "IOHandler.h"
#include "ClientSide.h"
#include "InetSocketAddress.h"
#include "HttpResponse.h"
#define STATUS_BLOCKING 1
#define STATUS_IDLE 2
class ClientSide;
class RemoteSide:public IOHandler
{
	public:
		RemoteSide();
		RemoteSide(InetSocketAddress*);
		~RemoteSide();
		int Connect();
		int GetState();
		int SetState(int);
		int Writeable();
		int Proccess();
		int SetStatusBlocking();
		int SetStatusIdle();
		int OnCanSend();
		int IsIdle();
		Stream* GetSendStream();
		InetSocketAddress* GetAddr();
		virtual int ProccessReceive(Stream*);
		virtual int ProccessSend();
		virtual int ProccessConnectionReset();
		int SetSendStream(Stream*);
		int SetClientSide(ClientSide*);
		HttpResponse* GetResponse();
	private:
		int m_iSocket;
		int m_iState;
		Stream* m_pStream;
		Stream* m_pSendStream;
		InetSocketAddress* m_pAddr;
		HttpResponse* m_pHttpResponse;
		ClientSide* m_pClientSide;
};
#endif
