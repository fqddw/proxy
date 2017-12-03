#include "NetEngineTask.h"

int NetEngineTask::Run()
{
	m_pNetEngine->Run();
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

NetEngineTask::NetEngineTask()
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
