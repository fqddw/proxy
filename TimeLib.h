#ifndef __TIME_LIB_H__
#define __TIME_LIB_H__
#include "CommonType.h"
#include "sys/time.h"
#include "time.h"
class Time
{
public:
	static struct timespec GetNow();
	static int Compare(struct timespec,struct timespec);
	static timespec Sub(struct timespec,struct timespec);
	static timespec Add(struct timespec tdst,struct timespec tsrc);
};
#endif
