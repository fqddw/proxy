#ifndef __DNS_FETCH_H__
#define __DNS_FETCH_H__
#include "IOHandler.h"

#include "Stream.h"

class DNSFetch: public IOHandler
{
	public:
		DNSFetch();
		int sendReq(char* url);
		Stream* getURLBuffer(char*);
		int getfirstip(char*);
		int ProccessReceive(Stream*);
		void SetMainTask(QueuedNetTask*);

		void SetRecvFlag();
		char* getUrl();
	private:
		char* m_pUrl;
};
#endif
