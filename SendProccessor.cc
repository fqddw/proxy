#include "SendProccessor.h"
#include "IOHandler.h"
#include "errno.h"
#include "Stream.h"
#include "sys/socket.h"
#include "CommonType.h"
#include "unistd.h"
#include "MemList.h"
#include "stdio.h"
#include "RemoteSide.h"
#include "NetEngineTask.h"

SendProccessor::SendProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler),Task()
{
}
#include "stdio.h"
#include "MemList.h"
extern MemList<void*>* pGlobalList;
int SendProccessor::Run()
{
	m_pIOHandler->ProccessSend();
	m_pIOHandler->ReleaseSendRefCount();
	m_pIOHandler->Release();
	NetEngineTask::getInstance()->GetNetEngine()->Lock();
	NetEngineTask::getInstance()->GetNetEngine()->ReduceTaskCount();
	int iTaskCount = NetEngineTask::getInstance()->GetNetEngine()->GetTaskCount();
	if(iTaskCount == 0)
	{
		NetEngineTask::getInstance()->IncCount();

		NetEngineTask::getInstance()->GetNetEngine()->GetMasterThread()->InsertTask(NetEngineTask::getInstance());
	}
	NetEngineTask::getInstance()->GetNetEngine()->Unlock();

	return TRUE;
}

SendProccessor::~SendProccessor()
{
}
