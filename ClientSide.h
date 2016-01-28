#ifndef __CLIENTSIDE_H__
#define __CLIENTSIDE_H__
#include "DataIOHandler.h"
#include "Stream.h"
#include "RemoteSide.h"
#include "HttpRequest.h"
#define CLIENT_STATE_IDLE 1
#define CLIENT_STATE_RUNNING 2
class RemoteSide;
class ClientSide : public DataIOHandler
{
	public:
		ClientSide();
		~ClientSide();
		ClientSide(int);
		int Proccess();
		RemoteSide* GetRemoteSide();
	private:
		int m_iState;
		Stream* m_pStream;
		HttpRequest* m_pHttpRequest;
		RemoteSide* m_pRemoteSide;
};
#endif
