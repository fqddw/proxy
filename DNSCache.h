#include "MemList.h"

class DNSItem
{
	public:
		int GetSaddr();
		char* GetHostName();
		void SetSaddr(int);
		void SetHostName(char*);
	private:
		int m_iSaddr;
		char* m_pHostName;
};
class DNSCache
{
	public:
		DNSCache();
		int getSaddrByHost(char*);
		int AddRecord(char*, int);
	private:
		MemList<DNSItem*>* m_pCacheData;
};
