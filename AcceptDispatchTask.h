#include "Task.h"
class AcceptDispatchTask
{
	public:
		AcceptDispatchTask();
		AcceptDispatchTask(Server*);
		int SetServer(Server*);
		int Run();
	private:
		Socket* m_pServer;
};

