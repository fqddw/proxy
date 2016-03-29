#ifndef __NETENGINE_H__
#define __NETENGINE_H__
#include "sys/epoll.h"
#include "IOHandler.h"
typedef struct epoll_event EPOLLEVENT;
class NetEngine
{
	public:
		int Init();
		int SetSize(int);
		int Loop();
		int AddFileDescriptor(IOHandler*,int);
		int ModFileDescriptor(IOHandler*,int);
		int RemoveFileDescriptor(IOHandler*);
	private:
		int m_iFD;
		int m_iSize;
};
#endif
