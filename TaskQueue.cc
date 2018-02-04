#include "TaskQueue.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

TaskQueue::TaskQueue() : cs_(new CriticalSection()),m_iSize(0),task_(new MemList<Task*>())
{
}

void TaskQueue::Insert(Task* pTask)
{
	cs_->Enter();
	int id = GetUniqueId();
	Task * pElement = pTask;
	pElement->SetId(id);
	/*vector<Task*>::iterator it = tasks_.begin();

	for(;it!=tasks_.end();it++)
	{
		if(Time::Compare((*it)->GetTime(),pTask->GetTime()))
		{
			break;
		}
	}
	//printf("CURSIZE %d\n", tasks_.size());
	tasks_.insert(it,pTask);*/

	MemNode<Task*>* pNode = task_->GetHead();
	if(pNode)
	{
	while(pNode->GetNext())
	{
		if(Time::Compare(pNode->GetData()->GetTime(), pTask->GetTime()))
		{
			break;
		}
		pNode = pNode->GetNext();
	}
	if(!pNode->GetNext())
		task_->Append(pTask);
	else
	{
		MemNode<Task*>* pNewNode = new MemNode<Task*>();
		pNewNode->SetData(pTask);
		pNewNode->SetNext(pNode->GetNext());
		pNode->SetNext(pNewNode);
	}
	}
	else
	{
		task_->Append(pTask);
	}
	m_iSize++;
	cs_->Leave();
}

Task* TaskQueue::GetLastestTask()
{
	cs_->Enter();
	Task* pTask = NULL;
	/*if(0 == tasks_.size())
		pTask = NULL;
	else
		pTask = tasks_.back();*/
	if(m_iSize == 0)
		pTask = NULL;
	else
		pTask = task_->GetEnd()->GetData();
	cs_->Leave();
	return pTask;
}

Task* TaskQueue::PopLastestTask()
{
	cs_->Enter();
	Task* pTask = NULL;
	/*if (0 == tasks_.size())
		pTask = NULL;
	else
	{
		pTask = tasks_.back();
		tasks_.pop_back();
	}*/
	if(m_iSize == 0)
		pTask = NULL;
	else
	{
		pTask = task_->GetEnd()->GetData();
		task_->Delete(pTask);
		m_iSize--;
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
	//int size = tasks_.size();
	int size = m_iSize;
	cs_->Leave();
	return size;
}
