#ifndef __SERVER_H__
#define __SERVER_H__
#include "sys/socket.h"
#include "netinet/in.h"
#include "netdb.h"
#include "IOHandler.h"
class Server : public IOHandler
{
	public:
		Server();
		int Create();
		void SetPort(int);
		int ProccessReceive();
		int AddToNetEngine();
	private:
		int m_iPort;
};
#endif
