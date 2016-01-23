#include "EventPump.h"
EventPump::EventPump()
{
	sem_init(&m_hSem,0,0);
	time_ = new struct timespec;
	time_->tv_sec = 0;
	time_->tv_nsec = 0;
}
void EventPump::SetDuring(struct timespec* time)
{
	time_->tv_sec = time->tv_sec;
	time_->tv_nsec = time->tv_nsec;
}
int EventPump::WaitObject()
{
	struct timespec ts;
	ts.tv_sec = time_->tv_sec;
	ts.tv_nsec = time_->tv_nsec;
	int ret = sem_timedwait(&m_hSem,&ts);
	return ret;
}


int EventPump::WakeUp()
{
	sem_post(&m_hSem);
	return 0;
}

int EventPump::WaitObjectForever()
{
	sem_wait(&m_hSem);
	return 0;
}
