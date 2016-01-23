#ifndef __IOEVENT_H__
#define __IOEVENT_H__
#include "sys/epoll.h"
#include "CommonType.h"
class IOHandler;
class NetEngine;
class IOEvent
{
	public:
		IOEvent();
		IOEvent(IOHandler*);
		int SetIOHandler(IOHandler*);
		void SetFD(int);
		int GetFD();
		IOHandler* GetHandler();
		struct epoll_event ToEpollEvent();
		NetEngine* GetNetEngine();
		void SetNetEngine(NetEngine* pNetEngine);
		int AddToEngine();
		int RemoveFromEngine();
	private:
		int m_iFD;
		IOHandler* m_pHandler;
		NetEngine* m_pEngine;
};

#endif
