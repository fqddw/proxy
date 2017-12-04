#include "NetEngineTask.h"

int NetEngineTask::Run()
{
	m_pNetEngine->Run();
	m_iCount--;
	return TRUE;
}

NetEngineTask* NetEngineTask::getInstance()
{
	if(!m_gpInstance)
	{
		m_gpInstance = new NetEngineTask();
	}
	return m_gpInstance;
}

NetEngineTask::NetEngineTask():m_iCount(1)
{
}

NetEngineTask::~NetEngineTask()
{
}

void NetEngineTask::SetEngine(NetEngine* pEngine)
{
	m_pNetEngine = pEngine;
}

NetEngine* NetEngineTask::GetNetEngine()
{
	return m_pNetEngine;
}

int NetEngineTask::GetCount()
{
	return m_iCount;
}

void NetEngineTask::IncCount()
{
	m_iCount = m_iCount + 1;
}
