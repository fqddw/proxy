#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
	pthread_mutex_init(&cs,NULL);
}

void CriticalSection::Enter()
{
	pthread_mutex_lock(&cs);
}

void CriticalSection::Leave()
{
	pthread_mutex_unlock(&cs);
}
CriticalSection::~CriticalSection()
{
	pthread_mutex_destroy(&cs);
}

