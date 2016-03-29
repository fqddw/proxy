#include "IOEvent.h"
#include "NetEngine.h"
IOEvent::IOEvent():m_iFD((int)NULL),m_pHandler(NULL),m_iOutReady(FALSE),m_iInReady(FALSE)
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

struct epoll_event IOEvent::ToEpollEvent(int events)
{
	struct epoll_event ee = {0};
	ee.events = events;
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

int IOEvent::AddToEngine(int events)
{
	m_pEngine->AddFileDescriptor(m_pHandler,events);
	return TRUE;
}

int IOEvent::RemoveFromEngine()
{
	m_pEngine->RemoveFileDescriptor(m_pHandler);
	return TRUE;
}
void IOEvent::SetInReady()
{
	m_iInReady = TRUE;
}

void IOEvent::CancelInReady()
{
	m_iInReady = FALSE;
}

int IOEvent::IsInReady()
{
	return m_iInReady;
}
void IOEvent::SetOutReady()
{
	m_iOutReady = TRUE;
}

void IOEvent::CancelOutReady()
{
	m_iOutReady = FALSE;
}

int IOEvent::IsOutReady()
{
	return m_iOutReady;
}
