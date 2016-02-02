#include "DataIOHandler.h"
#include "CommonType.h"
#include "sys/socket.h"
#include "unistd.h"
#include "errno.h"
#include "MemList.h"
#include "Stream.h"
extern MemList<void*>* pGlobalList;
DataIOHandler::DataIOHandler():IOHandler()
{
}

int DataIOHandler::GetDataStream(Stream** pStream)
{
	*pStream = NULL;
	for(;;)
	{
		char buffer[256*1024] = {'\0'};
		int n = recv(GetEvent()->GetFD(),buffer,1024,0);
		if(n < 0)
		{
			if(errno == EAGAIN)
			{
				return TRUE;
			}
			else if(errno == EINTR)
			{
				continue;
			}
			else
			{
				int sockfd = GetEvent()->GetFD();
				GetEvent()->RemoveFromEngine();
				if(pGlobalList->Delete(this))
				{
					delete this;
				}

				close(sockfd);
				return FALSE;
			}
		}
		if(n == 0)
		{
			int sockfd = GetEvent()->GetFD();
			GetEvent()->RemoveFromEngine();
			if(pGlobalList->Delete(this))
			{
				delete this;
			}
			close(sockfd);
			return FALSE;
		}
		if(*pStream == NULL)
			*pStream = new Stream();
		(*pStream)->Append(buffer,n);

	}
	return TRUE;
}
