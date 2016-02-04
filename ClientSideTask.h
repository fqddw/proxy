#include "Task.h"
#include "ClientSide.h"

class ClientSideTask : public Task
{
	public:
		ClientSideTask();
		ClientSideTask(ClientSide*);
		int SetClientSide(ClientSide*);
		int Run();
	private:
		ClientSide* m_pClientSide;
};
