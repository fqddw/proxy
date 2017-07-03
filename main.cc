#include "unistd.h"
#include "stdio.h"
#include "CommonType.h"
#include "NetEngine.h"
#include "ServerConfig.h"
#include "Server.h"
#include "MasterThread.h"
#include <signal.h>
#include "MemList.h"
#include "DNSCache.h"
void* threadProc(void* ptr)
{
	Task* pTaskWrapper = (Task*)ptr;
	pTaskWrapper->Run();
	return NULL;
}

class NetEngineInitTask : public Task
{
	public:
		NetEngineInitTask();
		NetEngineInitTask(NetEngine*);
		int Run();
		int SetNetEngine(NetEngine*);
	private:
		NetEngine* m_pNetEngine;
};

NetEngineInitTask::NetEngineInitTask():Task(),m_pNetEngine(NULL)
{
}

NetEngineInitTask::NetEngineInitTask(NetEngine* pNetEngine):Task()
{
	m_pNetEngine = pNetEngine;
}

int NetEngineInitTask::SetNetEngine(NetEngine* pNetEngine)
{
	m_pNetEngine = pNetEngine;
	return TRUE;
}
int NetEngineInitTask::Run()
{
	m_pNetEngine->Loop();
	return TRUE;
}

class Thread
{
	public:
		int Start();
		int SetTask(Task*);
	private:
		pthread_t m_hThread;
		Task* m_pTask;
};

int Thread::SetTask(Task* pTask)
{
	m_pTask = pTask;
	return TRUE;
}

MemList<void*>* pGlobalList = NULL;
DNSCache* g_pDNSCache = NULL;
#include "RemoteSide.h"
class ServerStartTask : public Task
{
	public:
		int Run();
		int SetNetEngine(NetEngine*);
		int SetMasterThread(MasterThread*);
	private:
		NetEngine* m_pEngine;
		MasterThread* m_pMasterThread;
};

int ServerStartTask::SetNetEngine(NetEngine* pEngine)
{
	m_pEngine = pEngine;
	return TRUE;
}
int Thread::Start()
{
	pthread_create(&m_hThread,NULL,threadProc,m_pTask);
	return TRUE;
}
int ServerStartTask::SetMasterThread(MasterThread* pMasterThread)
{
	m_pMasterThread = pMasterThread;
	return TRUE;
}
int ServerStartTask::Run()
{
	ServerConfigDefault* pConfig = new ServerConfigDefault();
	Server* pServer = new Server();
	//pGlobalList->Append(pServer);
	pServer->GetEvent()->SetNetEngine(m_pEngine);
	pServer->SetPort(pConfig->GetPort());
	pServer->SetMasterThread(m_pMasterThread);
	pServer->Create();
	pServer->SetCanWrite(FALSE);
	pServer->GetEvent()->AddToEngine(EPOLLET|EPOLLOUT|EPOLLIN|EPOLLERR);
	return TRUE;
}

MemList<RemoteSide*>* g_pGlobalRemoteSidePool;

int main(int argc,char** argv)
{	
	g_pGlobalRemoteSidePool = new MemList<RemoteSide*>();
	pGlobalList = new MemList<void*>();
	g_pDNSCache = new DNSCache();
	signal(SIGPIPE,SIG_IGN);
	NetEngine* pEngine = new NetEngine();
	pEngine->SetSize(1024);
	pEngine->Init();
	NetEngineInitTask* pNetEngineInitTask = new NetEngineInitTask();
	pNetEngineInitTask->CancelRepeatable();
	pNetEngineInitTask->SetNetEngine(pEngine);
	Thread* pThread = new Thread();
	pThread->SetTask(pNetEngineInitTask);
	pThread->Start();

	MasterThread* pMasterThread = new MasterThread(new EventPump());
	pMasterThread->SetWorkerCount(1);
	pMasterThread->Create();
	ServerStartTask* pServerStartTask = new ServerStartTask();
	pServerStartTask->CancelRepeatable();
	pServerStartTask->SetNetEngine(pEngine);
	pServerStartTask->SetMasterThread(pMasterThread);
	pMasterThread->InsertTask(pServerStartTask);
	while(1)
	{
		int c = getchar();
		if(c == 'l')
		{
			MemList<DNSItem*>* pList = g_pDNSCache->GetList();
			MemNode<DNSItem*>* pItem = pList->GetHead();
			while(pItem) {
				printf("%s\n", pItem->GetData()->GetHostName());
				pItem = pItem->GetNext();
			}
		}
		if(c=='q')
		{
						return 0;
		}
	}
	return 0;
}

