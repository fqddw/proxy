#ifndef __IOHANDLER_H__
#define __IOHANDLER_H__
#include "IOEvent.h"
#include "CommonType.h"
#include "MasterThread.h"
#include "ReceiveProccessor.h"
#include "SendProccessor.h"
#include "ConnectionResetProccessor.h"
#include "Stream.h"
#define HANDLER_IDLE 1
#define HANDLER_RUNNING 2
class SendProccessor;
class ReceiveProccessor;
class ConnectionResetProccessor;
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
		virtual int ProccessReceive(Stream*);
		virtual int ProccessSend();
		virtual int ProccessConnectionReset();
		virtual int IsServer();
	private:
		IOEvent* m_pEvent;
		MasterThread* m_pMasterThread;
		ReceiveProccessor* m_pRecvProc;
		ConnectionResetProccessor* m_pConnResetProc;
		SendProccessor* m_pSendProc;
		int m_bCanRead;
};


#endif
