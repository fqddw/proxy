#include "WorkThread.h"
DWORD ThreadProc(LPVOID ptr);
WorkThread::WorkThread(EventPump* ep) :ep_(ep), state_(STATE_RUNNING), tq_(new TaskQueue)
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
int WorkThread::Run()
{
	int flag = TRUE;
	do
	{
		Task* pTask=tq_->PopLastestTask();
		if (pTask)
		{
			pTask->Run();
			if(!pTask->Repeatable())
				delete pTask;
		}
		else
		{
			EnterWaiting();
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

int WorkThread::GetTaskCount()
{
	return this->tq_->GetSize();
}

int WorkThread::InsertTask(Task* pTask)
{
	tq_->Insert(pTask);
	if (IsWaiting())
		EnterRunning();
	return 0;
}
