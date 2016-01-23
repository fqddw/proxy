#include "IOEvent.h"
#include "NetEngine.h"
IOEvent::IOEvent():m_iFD((int)NULL),m_pHandler(NULL)
{
}
IOEvent::IOEvent(IOHandler* pHandler)
{
	m_pHandler = pHandler;
}
int IOEvent::SetIOHandler(IOHandler* pHandler)
{
	m_pHandler = pHandler;
	return TRUE;
}

struct epoll_event IOEvent::ToEpollEvent()
{
	struct epoll_event ee = {0};
	ee.events = EPOLLIN | EPOLLET;
	ee.data.ptr = m_pHandler;
	return ee;
}

int IOEvent::GetFD()
{
	return m_iFD;
}

void IOEvent::SetFD(int fd)
{
	m_iFD = fd;
}
NetEngine* IOEvent::GetNetEngine()
{
	return m_pEngine;
}

void IOEvent::SetNetEngine(NetEngine* pNetEngine)
{
	m_pEngine = pNetEngine;
}

int IOEvent::AddToEngine()
{
	m_pEngine->AddFileDescriptor(m_pHandler);
	return TRUE;
}

int IOEvent::RemoveFromEngine()
{
	m_pEngine->RemoveFileDescriptor(m_pHandler);
	return TRUE;
}
