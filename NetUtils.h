#ifndef __NETUTILS_H__
#define __NETUTILS_H__
#include "InetSocketAddress.h"
class NetUtils
{
	public:
		NetUtils();
		~NetUtils();
		static InetSocketAddress* GetHostByName(char*,int);
};
#endif
