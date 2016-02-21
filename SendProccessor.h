#ifndef __SENDPROCCESSOR_H__
#define __SENDPROCCESSOR_H__
#include "Task.h"
#include "IOHandler.h"
class SendProccessor : public Task
{
	public:
		SendProccessor();
		SendProccessor(IOHandler*);
		~SendProccessor();

	private:
		IOHandler* m_pIOHandler;
	public:
		int Run();
};
#endif
