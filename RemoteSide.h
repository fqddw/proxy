#ifndef __REMOTESIDE_H__
#define __REMOTESIDE_H__

#include "CommonType.h"
#include "IOHandler.h"
#include "ClientSide.h"
#include "InetSocketAddress.h"
#include "HttpResponse.h"
#define STATUS_BLOCKING 1
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
		int OnCanSend();
		Stream* GetSendStream();
		InetSocketAddress* GetAddr();
		virtual int ProccessReceive(Stream*);
		virtual int ProccessSend();
		virtual int ProccessConnectionReset();
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
