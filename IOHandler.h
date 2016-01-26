#ifndef __IOHANDLER_H__
#define __IOHANDLER_H__
#include "IOEvent.h"
#include "CommonType.h"
#include "MasterThread.h"
#define HANDLER_IDLE 1
#define HANDLER_RUNNING 2
class IOHandler:public Task
{
	public:
		IOHandler();
		~IOHandler();
		IOEvent* GetEvent();
		int GetRunningStatus();
		void SetRunningStatus(int);
		int SetMasterThread(MasterThread*);
		MasterThread* GetMasterThread();
		int Run();
		virtual int Proccess();
		int Dispatch();
	private:
		int m_iStatus;
		IOEvent* m_pEvent;
		MasterThread* m_pMasterThread;
};


#endif
