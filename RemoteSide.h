#ifndef __REMOTESIDE_H__
#define __REMOTESIDE_H__

#include "CommonType.h"
#include "DataIOHandler.h"
#include "ClientSide.h"
class ClientSide;
class RemoteSide:public DataIOHandler
{
	public:
		RemoteSide();
		~RemoteSide();
		int GetState();
		int SetState(int);
		int WriteData();
		int Writeable();
		int Proccess();
	private:
		int m_iSocket;
		int m_iState;
		Stream* m_pStream;
		ClientSide* m_pClientSide;
};
#endif
