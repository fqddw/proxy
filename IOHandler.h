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
#define REMOTE_SIDE 1
#define CLIENT_SIDE 2

#define STATE_NORMAL 0
#define STATE_RUNNING 1
#define STATE_ABORT 2
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
		void SetCanWrite(int);
		void SetCanRead(int);
		int CanRead();
		int CanWrite();
		Task* GetRecvTask();
		Task* GetSendTask();
		int LockSendBuffer();
		int UnlockSendBuffer();
		virtual int GetSide();
		virtual Stream* GetSendStream();
		virtual int ClearHttpEnd(){return 0;};
		int IsClosed();
		void SetClosed(int);
	private:
		IOEvent* m_pEvent;
		MasterThread* m_pMasterThread;
		ConnectionResetProccessor* m_pConnResetProc;
		int m_bCanRead;
		int m_bCanWrite;
		CriticalSection* cs_;
		int m_bClosed;
};


#endif
