#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include "EventPump.h"
#include "TaskQueue.h"

#define WORK_THREAD_COUNT 8
#define STATE_WAITING 0
#define STATE_RUNNING 1
class MasterThread;
class WorkThread
{
public:
	WorkThread(EventPump*);
	~WorkThread();
private:
	EventPump* ep_;
	Task* task_;
	int state_;
	MasterThread* pMasterThread_;
public:
	int GetState();
	int Run();
	void EnterWaiting();
	void EnterRunning();
	int IsWaiting();
	int IsRunning();
	int SetMasterThread(MasterThread*);
	int HasTask();
	int SetTask(Task*);
};
#endif
