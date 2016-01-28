#ifndef __NETUTILS_H__
#define __NETUTILS_H__
#include "InetSocketAddress.h"
class NetUtils
{
	public:
		NetUtils();
		~NetUtils();
	private:
		InetSocketAddress* GetHostByName(char*);
};
#endif
