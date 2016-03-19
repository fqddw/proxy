#include "ConnectionResetProccessor.h"
#include "IOHandler.h"
#include "errno.h"
#include "Stream.h"
#include "sys/socket.h"
#include "CommonType.h"
#include "unistd.h"
#include "MemList.h"
extern MemList<void*>* pGlobalList;
ConnectionResetProccessor::ConnectionResetProccessor(IOHandler* pIOHandler):m_pIOHandler(pIOHandler)
{
}

int ConnectionResetProccessor::Run()
{
	m_pIOHandler->GetEvent()->RemoveFromEngine();
	close(m_pIOHandler->GetEvent()->GetFD());
	delete m_pIOHandler;
	return 0;
}

ConnectionResetProccessor::~ConnectionResetProccessor()
{
}
