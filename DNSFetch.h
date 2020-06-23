#ifndef __DNS_FETCH_H__
#define __DNS_FETCH_H__


class DNSFetch: public IOHandler
{
	public:
		DNSFetch();
		int sendReq(char* url);
	private:
};
#endif
