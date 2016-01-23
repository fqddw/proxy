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
    vector<Task*> tasks_;

	CriticalSection * cs_;
private:
    int GetUniqueId();
};


#endif	/* TASKQUEUE_H */

