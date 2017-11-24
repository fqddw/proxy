#include "SendProccessor.h"
SendProccessor::SendProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler),Task()
{
}
#include "stdio.h"
#include "MemList.h"
extern MemList<void*>* pGlobalList;
int SendProccessor::Run()
{
	m_pIOHandler->ProccessSend();
	m_pIOHandler->ReleaseSendRefCount();
	m_pIOHandler->Release();
	return 0;
}

SendProccessor::~SendProccessor()
{
}
