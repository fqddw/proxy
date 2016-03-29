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
		struct epoll_event ToEpollEvent(int);
		NetEngine* GetNetEngine();
		void SetNetEngine(NetEngine* pNetEngine);
		int AddToEngine(int);
		int RemoveFromEngine();
		void SetInReady();
		void CancelInReady();
		int IsInReady();
		void SetOutReady();
		void CancelOutReady();
		int IsOutReady();

	private:
		int m_iInReady;
		int m_iOutReady;
		int m_iFD;
		int m_bCanWrite;
		IOHandler* m_pHandler;
		NetEngine* m_pEngine;
};

#endif
