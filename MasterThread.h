#ifndef MASTERTHREAD_H
#define MASTERTHREAD_H
#include "EventPump.h"
#include "TaskQueue.h"
#include "CriticalSection.h"
#include "TimeLib.h"
#include "CommonType.h"
#include "pthread.h"
#define STATE_WAITING 0
#define STATE_RUNNING 1
class WorkThread;
class MasterThread
{
public:
	MasterThread(EventPump*);
	int Init();
	int WakeUp();
	int Sleep();
	int SleepForever();
//	void SetDuring(DWORD time);
	int Create();
	int RunLoop();
	int DispatchTask(Task* pTask);
	void EnterWaiting();
	void EnterRunning();
	int IsWaiting();
	int IsRunning();
	int InsertTask(Task* pTask);
	void SetWorkerCount(int);
	int SetWorkThreadBusy(int);
	int GetWorkerCount();
private:
	EventPump* ep_;
	TaskQueue * pTaskQueue;
	vector<WorkThread*> pWorkThread_;
	int state_;
	int m_iWorkCount;
	int workthread_busy;
};
#define WORKTHREAD_BUSY 1
#define WORKTHREAD_IDLE 2

#endif
