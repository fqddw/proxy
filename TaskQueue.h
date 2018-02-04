/* 
 * File:   TaskQueue.h
 * Author: fqddw
 *
 * Created on June 29, 2015, 2:49 PM
 */

#ifndef TASKQUEUE_H
#define	TASKQUEUE_H

#include "Task.h"
#include "CriticalSection.h"
#include "TimeLib.h"
#include "MemList.h"
using namespace std;
class TaskQueue
{
public:
    TaskQueue();
public:
    Task* GetLastestTask();
	Task* PopLastestTask();
    void Insert(Task*);
	int GetSize();
private:
    //vector<Task*> tasks_;
    MemList<Task*>* task_;
    int m_iSize;

	CriticalSection * cs_;
private:
    int GetUniqueId();
};


#endif	/* TASKQUEUE_H */

