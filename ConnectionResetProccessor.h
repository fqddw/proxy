#ifndef __CONNECTIONRESETPROCCESSOR_H__
#define __CONNECTIONRESETPROCCESSOR_H__
#include "Task.h"
#include "IOHandler.h"
#include "Stream.h"
class ConnectionResetProccessor : public Task
{
	public:
		ConnectionResetProccessor();
		ConnectionResetProccessor(IOHandler*);
		~ConnectionResetProccessor();
	public:
		int Run();
	private:
		IOHandler* m_pIOHandler;
		int m_iState;
};
#endif

