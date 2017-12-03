/* 
 * File:   Task.h
 * Author: fqddw
 *
 * Created on June 29, 2015, 1:44 PM
 */

#ifndef TASK_H
#define	TASK_H
#include "vector"
#include "CommonType.h"
#include "sys/time.h"
using namespace std;
class Task
{
public:
	Task();
	virtual ~Task();
public:
	void SetTime(struct timespec* time);
	struct timespec GetTime();
    void SetId(int);
	virtual int Run();
	int Repeatable();
	void SetRepeatable();
	void CancelRepeatable();
	void Release();
private:
	int m_bDeleted;
	int m_iRepeatable;
	struct timespec timestamp_;
    int id_;
};

#endif	/* TASK_H */

