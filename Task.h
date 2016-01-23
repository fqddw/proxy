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
public:
	void SetTime(struct timespec* time);
	struct timespec GetTime();
    void SetId(int);
	virtual int Run();
	int Repeatable();
	void SetRepeatable();
	void CancelRepeatable();
private:
	int m_iRepeatable;
	struct timespec timestamp_;
    int id_;
};

#endif	/* TASK_H */

