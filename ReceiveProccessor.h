#ifndef __RECEIVEPROCCESSOR_H__
#define __RECEIVEPROCCESSOR_H__
#include "Task.h"
#include "IOHandler.h"
class ReceiveProccessor : public Task
{
	public:
		ReceiveProccessor();
		ReceiveProccessor(IOHandler*);
		~ReceiveProccessor();
	public:
		int Run();
	private:
		IOHandler* m_pIOHandler;
		int m_iState;
};
#endif

