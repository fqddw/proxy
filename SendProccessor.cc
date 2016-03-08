#include "SendProccessor.h"
SendProccessor::SendProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler)
{
}
#include "stdio.h"
int SendProccessor::Run()
{
	printf("Send Task %d %d\n",m_pIOHandler,m_pIOHandler->GetEvent()->GetFD());
	m_pIOHandler->ProccessSend();
	return 0;
}
