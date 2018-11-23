#ifndef __NETTASK_H__
#define __NETTASK_H__
#include "CommonType.h"

#include "Task.h"
#include "IOHandler.h"
#define EVENT_RECVING 1
#define NET_SENDING 2
class NetTask : public Task
{
	private:
		int m_bRecving;
		int m_bSending;
		CriticalSection* cs_;
		int m_bRunning;
		int m_iCount;
	public:
		NetTask();
		virtual ~NetTask();
		int Run();
		void Lock();
		void Unlock();
		int GetNextTask();
		int IsRunning();
		void SetRecving();
		void SetSending();
		int GetDataStream(IOHandler*, Stream**);
		void SetRunning();
		int IssetRecving();
		void IncCount();
};
#endif
