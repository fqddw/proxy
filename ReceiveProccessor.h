#include "Task.h"
class ReceiveProccessor : public Task
{
	public:
		ReceiveProccessor();
		~ReceiveProccessor();
	public:
		int Run();
	private:
		int m_iState;
};
