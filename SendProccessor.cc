#include "SendProccessor.h"
SendProccessor::SendProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler),Task()
{
}
#include "stdio.h"
int SendProccessor::Run()
{
	m_pIOHandler->ProccessSend();
	m_pIOHandler->ReleaseSendRefCount();
	m_pIOHandler->Release();
	return 0;
}
