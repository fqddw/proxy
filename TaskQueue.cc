#include "TaskQueue.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

TaskQueue::TaskQueue() : cs_(new CriticalSection())
{
}

void TaskQueue::Insert(Task* pTask)
{
	printf("Insert Task %d\n", GetSize());
	cs_->Enter();
	int id = GetUniqueId();
	Task * pElement = pTask;
	pElement->SetId(id);
	vector<Task*>::iterator it = tasks_.begin();

	for(;it!=tasks_.end();it++)
	{
		if(Time::Compare((*it)->GetTime(),pTask->GetTime()))
		{
			break;
		}
	}
	tasks_.insert(it,pTask);
	cs_->Leave();
}

Task* TaskQueue::GetLastestTask()
{
	cs_->Enter();
	Task* pTask = NULL;
	if(0 == tasks_.size())
		pTask = NULL;
	else
		pTask = tasks_.back();
	cs_->Leave();
	return pTask;
}

Task* TaskQueue::PopLastestTask()
{
	cs_->Enter();
	Task* pTask = NULL;
	if (0 == tasks_.size())
		pTask = NULL;
	else
	{
		pTask = tasks_.back();
		tasks_.pop_back();
	}
	cs_->Leave();
	return pTask;
}


int TaskQueue::GetUniqueId()
{
	int id = rand();
	return id;
}

int TaskQueue::GetSize()
{
	cs_->Enter();
	int size = tasks_.size();
	cs_->Leave();
	return size;
}
