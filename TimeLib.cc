#include "TimeLib.h"
struct timespec Time::GetNow()
{
	struct timespec t;
	clock_gettime(CLOCK_REALTIME,&t);
	return t;
}

int Time::Compare(struct timespec dst,struct timespec src)
{
	if(dst.tv_sec>src.tv_sec)
		return 1;
	if(dst.tv_sec<src.tv_sec)
		return -1;
	if(dst.tv_sec == src.tv_sec)
	{
		if(dst.tv_nsec>src.tv_nsec)
			return 1;
		if(dst.tv_nsec<src.tv_nsec)
			return -1;
		else
			return 0;
	}
}

struct timespec Time::Sub(struct timespec dst,struct timespec src)
{
	struct timespec t;
	if(dst.tv_nsec - src.tv_nsec<0){
		t.tv_nsec = 1000000000+dst.tv_nsec - src.tv_nsec;
		t.tv_sec = dst.tv_sec - src.tv_sec-1;
	}
	else
	{
		t.tv_nsec = dst.tv_nsec - src.tv_nsec;
		t.tv_sec = dst.tv_sec - src.tv_sec;
	}

	return t;
}
struct timespec Time::Add(struct timespec dst,struct timespec src)
{
	struct timespec t;
	t.tv_sec=dst.tv_sec+src.tv_sec;
	t.tv_nsec=dst.tv_nsec+src.tv_nsec;
	if(t.tv_nsec>1000000000)
	{
		t.tv_sec += t.tv_nsec/1000000000;
		t.tv_nsec %= 1000000000;
	}
	return t;
}
