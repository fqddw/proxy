#include "MemList.h"

class DNSItem
{
	public:
		int GetSaddr();
		char* GetHostName();
		int IsValid();
		void SetSaddr(int);
		void SetHostName(char*);
		void SetValid(int);
	private:
		int m_bValid;
		int m_iSaddr;
		char* m_pHostName;
};
class DNSCache
{
	public:
		DNSCache();
		int getSaddrByHost(char*);
		DNSItem* getItemByHost(char*);
		int AddRecord(char*, int, int);
		MemList<DNSItem*>* GetList();
	private:
		MemList<DNSItem*>* m_pCacheData;
};
