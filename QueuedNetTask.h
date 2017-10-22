#ifndef __QUEUEDNETTASK_H__
#include "CommonType.h"

#include "Task.h"
#include "ClientSide.h"
#include "RemoteSide.h"
#define CLIENT_RECVING 1
#define CLIENT_SENDING 2
#define REMOTE_RECVING 3
#define REMOTE_SENDING 4
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
	public:
		QueuedNetTask();
		int Run();
		int GetNextTask();
};
#endif
