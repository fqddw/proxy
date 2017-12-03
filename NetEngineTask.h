#ifndef __NETENGINETASK_H__
#define __NETENGINETASK_H__

#include "Task.h"
#include "NetEngine.h"
class NetEngineTask : public Task
{
	public:
		static NetEngineTask* getInstance();
		int Run();
		void SetEngine(NetEngine*);
		NetEngine* GetNetEngine();
	private:
		NetEngineTask();
		~NetEngineTask();
	private:
		static NetEngineTask* m_gpInstance;
		NetEngine* m_pNetEngine;
};

#endif
