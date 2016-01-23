#include "Task.h"

struct timespec Task::GetTime()
{
    return timestamp_;
}

void Task::SetId(int id)
{
    id_=id;
}

int Task::Run()
{
	return 0;
}

void Task::SetTime(struct timespec* time)
{
	timestamp_.tv_sec = time->tv_sec;
	timestamp_.tv_nsec = time->tv_nsec;
}
Task::Task() :  m_iRepeatable(FALSE)
{
timestamp_.tv_sec=0;
timestamp_.tv_nsec=0;
}
int Task::Repeatable()
{
	return m_iRepeatable;
}

void Task::SetRepeatable()
{
	m_iRepeatable =TRUE;
}
void Task::CancelRepeatable()
{
	m_iRepeatable = FALSE;
}
