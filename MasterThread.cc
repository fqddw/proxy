#include "MasterThread.h"
#include "TaskQueue.h"
#include "string"
#include "WorkThread.h"
#include "TimeLib.h"
#include "CommonType.h"


LPVOID ThreadProc(LPVOID ptr)
{
	MasterThread* pThread = (MasterThread*)ptr;
	pThread->RunLoop();
	return 0;
}
LPVOID WorkThreadProc(LPVOID ptr)
{
	WorkThread* pThread = (WorkThread*)ptr;
	pThread->Run();
	return 0;
}
MasterThread::MasterThread(EventPump* ep) :ep_(ep), pTaskQueue(new TaskQueue), state_(STATE_RUNNING),workthread_busy(WORKTHREAD_IDLE)
{

}
/*
void MasterThread::SetDuring(DWORD time)
{
	ep_->SetDuring(time);
}
*/
int MasterThread::Sleep()
{
	if (state_ == STATE_RUNNING)
	{
		state_ = STATE_WAITING;
		ep_->WaitObject();
		state_ = STATE_RUNNING;
	}
	return 0;
}
int MasterThread::SleepForever()
{
	if (state_ == STATE_RUNNING)
	{
		state_ = STATE_WAITING;
		ep_->WaitObjectForever();
		state_ = STATE_RUNNING;
	}
	return 0;
}
int MasterThread::WakeUp()
{
	ep_->WakeUp();
	return 0;
}
int MasterThread::RunLoop()
{
	Task* pNextTask = NULL;
	while (1)
	{
		if (workthread_busy != WORKTHREAD_BUSY)
		{
			pNextTask = pTaskQueue->PopLastestTask();
			if(pNextTask == NULL)
			{
				SleepForever();
			}
			else
			{
				struct timespec time = Time::GetNow();
				int cmp = Time::Compare(time,pNextTask->GetTime());
				if (cmp >= 0)
				{
					int ret = DispatchTask(pNextTask);
					if(!ret)
					{
						workthread_busy = WORKTHREAD_BUSY;
						InsertTask(pNextTask);
						SleepForever();
						continue;
					}
					//pTaskQueue->PopLastestTask();
				}
				else
				{
					struct timespec taskTime = pNextTask->GetTime();
					InsertTask(pNextTask);
					ep_->SetDuring(&taskTime);
					Sleep();
				}
			}
		}
		else
		{
			SleepForever();
		}
	}
	return 0;
}

void MasterThread::SetWorkerCount(int count)
{
	m_iWorkCount = count;
}
int MasterThread::GetWorkerCount()
{
	return m_iWorkCount;
}

int MasterThread::Create()
{
	pthread_t main;
	pthread_create(&main, 0, ThreadProc, this);
	int i = 0;
	
	for (; i < GetWorkerCount(); i++)
	{
		WorkThread* pWorkThread = new WorkThread(new EventPump());
		pWorkThread->SetMasterThread(this);
		pthread_t t;
		pthread_create(&t, NULL, WorkThreadProc, pWorkThread);
		pWorkThread_.push_back(pWorkThread);
	}
	return 0;
}

int MasterThread::DispatchTask(Task* pTask)
{
	WorkThread* pWorkThreadMin = NULL;
	int count = 0;
	int flag = 0;
	vector<WorkThread*>::iterator it = pWorkThread_.begin();
	for (; it != pWorkThread_.end();it++)
	{
/*		if ((*it)->IsRunning())
		{
			int cur_count = (*it)->GetTaskCount();
			if (flag == 0)
			{
				count = cur_count;
				pWorkThreadMin = *it;
				flag = 1;
			}
			else
			{
				if (count > cur_count)
				{
					pWorkThreadMin = *it;
					count = cur_count;
				}
			}
		}
		else
		{
			pWorkThreadMin = (*it);
			break;
		}*/
		if (!(*it)->IsRunning()){
			(*it)->SetTask(pTask);
			return TRUE;
		}
	}
	return FALSE;
}

int MasterThread::InsertTask(Task* pTask)
{
	pTaskQueue->Insert(pTask);
	EnterRunning();
	return 0;
}
void MasterThread::EnterWaiting()
{
	if (state_ == STATE_RUNNING)
	{
		state_ = STATE_WAITING;
		ep_->WaitObjectForever();
		state_ = STATE_RUNNING;
	}
}

int MasterThread::IsWaiting()
{
	return state_ == STATE_WAITING;
}

int MasterThread::IsRunning()
{
	return state_ == STATE_RUNNING;
}
void MasterThread::EnterRunning()
{
	if (state_ == STATE_WAITING)
	{
		state_ = STATE_RUNNING;
		ep_->WakeUp();
	}
}
int MasterThread::SetWorkThreadBusy(int state)
{
	workthread_busy = state;
	return TRUE;
}
