#ifndef _EVENTPUMP_H_
#define _EVENTPUMP_H_
#include "sys/time.h"
#include <semaphore.h>
#include "CommonType.h"
class EventPump
{
public:
	EventPump();
	int WaitObject();
	int WakeUp();
	int WaitObjectForever();
	void SetDuring(struct timespec*);
private:
	sem_t m_hSem;
	struct timespec* time_;
};
#endif
