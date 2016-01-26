#ifndef __CLIENTSIDE_H__
#define __CLIENTSIDE_H__
#include "DataIOHandler.h"
#include "Stream.h"
#define CLIENT_STATE_IDLE 1
#define CLIENT_STATE_RUNNING 2
class ClientSide : public DataIOHandler
{
	public:
		ClientSide();
		~ClientSide();
		ClientSide(int);
		int Proccess();
	private:
		int m_iState;
		Stream* m_pStream;
};
#endif
