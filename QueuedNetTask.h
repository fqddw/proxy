#ifndef __QUEUEDNETTASK_H__
#define __QUEUEDNETTASK_H__
#include "CommonType.h"

#include "Task.h"
#include "IOHandler.h"
#include "ClientSide.h"
#include "RemoteSide.h"
#define CLIENT_RECVING 1
#define CLIENT_SENDING 2
#define REMOTE_RECVING 3
#define REMOTE_SENDING 4
#define DNS_ARRIVE 5
class QueuedNetTask : public Task
{
	private:
		ClientSide* m_pClientSide;
		int m_bClientRecving;
		int m_bClientSending;
		RemoteSide* m_pRemoteSide;
		int m_bRemoteRecving;
		int m_bRemoteSending;
		CriticalSection* cs_;
		int m_bRunning;
		int m_iCount;
	public:
		QueuedNetTask();
		virtual ~QueuedNetTask();
		int Run();
		void Lock();
		void Unlock();
		int GetNextTask();
		int IsRunning();
		void SetClient(ClientSide*);
		void SetRemote(RemoteSide*);
		ClientSide* GetClientSide();
		RemoteSide* GetRemoteSide();
		void SetClientRecving();
		void SetClientSending();
		void SetRemoteRecving();
		void SetRemoteSending();
		int GetDataStream(IOHandler*, Stream**);
		void SetRunning();
		int IssetClientRecving();
		int IssetRemoteRecving();
		void IncCount();
};
#endif
