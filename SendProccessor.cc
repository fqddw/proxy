#include "SendProccessor.h"
SendProccessor::SendProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler)
{
}
#include "stdio.h"
int SendProccessor::Run()
{
	m_pIOHandler->ProccessSend();
	return 0;
}
