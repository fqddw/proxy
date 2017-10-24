#include "QueuedNetTask.h"

int QueuedNetTask::Run()
{
}

int QueuedNetTask::GetNextTask()
{
	cs_->Enter();
	if(m_pClientSide)
	{
		if(m_bClientRecving)
		{
			if(!IsServer())
			{
				Stream* pStream = NULL; 
				GetDataStream(&pStream);
				m_pClientSide->ProccessReceive(pStream);
			}
		}
		if(m_bClientSending)
		{
		}
	}
	if(m_pRemoteSide)
	{
		if(m_bRemoteRecving)
		{
		}
		if(m_bRemoteSending)
		{
		}
	}
	cs_->Leave();
}


QueuedNetTask::QueuedNetTask():m_pClientSide(NULL),m_pRemoteSide(NULL),m_bClientRecving(FALSE),m_bClientSending(FALSE),m_bRemoteRecving(FALSE),m_bRemoteSending(FALSE)
{
}
