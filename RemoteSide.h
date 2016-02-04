#ifndef __REMOTESIDE_H__
#define __REMOTESIDE_H__

#include "CommonType.h"
#include "DataIOHandler.h"
#include "ClientSide.h"
#include "InetSocketAddress.h"
#define STATUS_BLOCKING 1
class ClientSide;
class RemoteSide:public DataIOHandler
{
	public:
		RemoteSide();
		RemoteSide(InetSocketAddress*);
		~RemoteSide();
		int Connect();
		int GetState();
		int SetState(int);
		int WriteData();
		int Writeable();
		int Proccess();
		int SetStatusBlocking();
		InetSocketAddress* GetAddr();
	private:
		int m_iSocket;
		int m_iState;
		Stream* m_pStream;
		InetSocketAddress* m_pAddr;
		ClientSide* m_pClientSide;
};
#endif
