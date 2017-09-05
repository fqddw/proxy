#ifndef __NETENGINE_H__
#define __NETENGINE_H__
#include "sys/epoll.h"
#include "IOHandler.h"
#define MAX_WAIT 1024
#define INFINITE -1

typedef struct epoll_event EPOLLEVENT;
class NetEngine
{
	public:
		NetEngine();
		int Init();
		int SetSize(int);
		int Loop();
		int AddFileDescriptor(IOHandler*,int);
		int ModFileDescriptor(IOHandler*,int);
		int RemoveFileDescriptor(IOHandler*);
		void Lock();
		void Unlock();
	private:
		int m_iFD;
		int m_iSize;
		CriticalSection* cs_;
		int m_iNFDS;
		EPOLLEVENT m_pEvents[MAX_WAIT];
			
};
#endif
