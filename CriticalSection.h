#ifndef CRITICALSECTION_H
#define CRITICALSECTION_H
#include "pthread.h"
class CriticalSection
{
private:
	pthread_mutex_t cs;
public:
	CriticalSection();
	void Enter();
	void Leave();
	~CriticalSection();
};
#endif
