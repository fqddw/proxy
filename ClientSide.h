#ifndef __CLIENTSIDE_H__
#define __CLIENTSIDE_H__
#include "IOHandler.h"

#define CLIENT_STATE_IDLE 1
#define CLIENT_STATE_RUNNING 2
class ClientSide : public IOHandler
{
	public:
		ClientSide();
		ClientSide(int);
		int Proccess();
		int Run();
	private:
		int m_iState;
};
#endif
