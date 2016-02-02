#include "CommonType.h"
#include "MemList.h"
#include "InetSocketAddress.h"
MemList<RemoteSide*>* g_pGlobalRemoteSidePool = NULL;

class RemoteSidePool
{
	public:
		RemoteSidePool();
		RemoteSide* FindByInetSockaddr(InetSocketAddress*);
};

RemoteSidePool::RemoteSidePool()
{
}

RemoteSide* RemoteSidePool::FindByInetSockaddr(InetSocketAddress* pAddr)
{
}
