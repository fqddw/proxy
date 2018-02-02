#ifndef __ADMIN_SERVER_H__
#define __ADMIN_SERVER_H__

#include "Task.h"
#include "IOHandler.h"
class AdminServer : public IOHandler
{
	public:
		AdminServer();
		int Create();
		void SetPort(int);
		int ProccessReceive(Stream*);
		int AddToNetEngine();
		int IsServer();
	private:
		int m_iPort;
};

class AdminServerStartTask : public Task
{
	public:
		int Run();
		int SetNetEngine(NetEngine*);
		int SetMasterThread(MasterThread*);
	private:
		NetEngine* m_pEngine;
		MasterThread* m_pMasterThread;
};

#endif
