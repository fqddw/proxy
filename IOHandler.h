#ifndef __IOHANDLER_H__
#define __IOHANDLER_H__
#include "IOEvent.h"
#include "CommonType.h"
#include "MasterThread.h"
#include "ReceiveProccessor.h"
#include "SendProccessor.h"
#include "Stream.h"
#define HANDLER_IDLE 1
#define HANDLER_RUNNING 2
class IOHandler
{
	public:
		IOHandler();
		~IOHandler();
		IOEvent* GetEvent();
		int SetMasterThread(MasterThread*);
		MasterThread* GetMasterThread();
		int Run();
		virtual int Proccess();
		int Dispatch(int);
		int ProccessReceive(Stream*);
		int ProccessSend();
		int ProccessConnectionReset();
	private:
		IOEvent* m_pEvent;
		MasterThread* m_pMasterThread;
		ReceiveProccessor* m_pRecvProc;
		SendProccessor* m_pSendProc;
};


#endif
