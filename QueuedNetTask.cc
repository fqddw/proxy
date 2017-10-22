#include "QueuedNetTask.h"

int QueuedNetTask::Run()
{
}

int QueuedNetTask::GetNextTask()
{
	cs_->Enter();
	cs_->Leave();
}


QueuedNetTask::QueuedNetTask():m_pClientSide(NULL),m_pRemoteSide(NULL),m_bClientRecving(FALSE),m_bClientSending(FALSE),m_bRemoteRecving(FALSE),m_bRemoteSending(FALSE)
{
}
