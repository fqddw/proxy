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
class RemoteSide;
class ClientSide : public IOHandler
{
	public:
		ClientSide();
		~ClientSide();
		ClientSide(int);
		int Proccess();
		RemoteSide* GetRemoteSide(InetSocketAddress*);
		int ProccessReceive(Stream*);
		int ProccessConnectionReset();
		int ProccessSend();
		Stream* GetSendStream();
		int WriteData();
		void SetTransIdleState();
		HttpRequest* GetRequest();
		int ClearHttpEnd();
		int GetSide();
	private:
		int m_iState;
		int m_iTransState;
		Stream* m_pStream;
		Stream* m_pSendStream;
		HttpRequest* m_pHttpRequest;
		RemoteSide* m_pRemoteSide;
};
#endif
