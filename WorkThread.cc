#include "MasterThread.h"
#include "WorkThread.h"

#include "stdio.h"
DWORD ThreadProc(LPVOID ptr);
WorkThread::WorkThread(EventPump* ep) :ep_(ep), state_(STATE_RUNNING), task_(NULL)
{

}

void WorkThread::EnterWaiting()
{
	if (state_ == STATE_RUNNING)
	{
		state_ = STATE_WAITING;
		ep_->WaitObjectForever();
		state_ = STATE_RUNNING;
	}
}

int WorkThread::IsWaiting()
{
	return state_ == STATE_WAITING;
}

int WorkThread::IsRunning()
{
	return state_ == STATE_RUNNING;
}
void WorkThread::EnterRunning()
{
	if (state_ == STATE_WAITING)
	{
		state_ = STATE_RUNNING;
		ep_->WakeUp();
	}
}
#include "MemList.h"
extern MemList<void*>* pGlobalList;
int WorkThread::Run()
{
	int flag = TRUE;
	do
	{
		Task* pTask = task_;
		if (pTask)
		{
			int bRepeatable	= pTask->Run();
			if(!bRepeatable){
				//if(pGlobalList->Delete(pTask))
				{
					delete pTask;
				}
			}
			task_ = NULL;
		}
		else
		{
			pMasterThread_->RunningLock();
			state_ = STATE_WAITING;
			pMasterThread_->SetWorkThreadBusy(WORKTHREAD_IDLE);
			pMasterThread_->WakeUp();
			pMasterThread_->RunningUnlock();
			ep_->WaitObjectForever();
		}
	}
	while (flag == TRUE);
	return 0;
}

int WorkThread::SetMasterThread(MasterThread* pThread)
{
	pMasterThread_ = pThread;
	return 0;
}

int WorkThread::HasTask()
{
	return task_ == NULL;
}

int WorkThread::SetTask(Task* pTask)
{
	task_ = pTask;
	if (IsWaiting())
		EnterRunning();
	return 0;
}
