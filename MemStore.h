#ifndef __MEM_STORE_H__
#define __MEM_STORE_H__
#include "MemList.h"
#include "Stream.h"
class StoreItem
{
	private:
		Stream* m_pHost;
		Stream* m_pRequestURL;
		Stream* m_pResponse;
	public:
		StoreItem();
		StoreItem(Stream*,Stream*);
		Stream* GetHost();
		Stream* GetRequestURL();
		Stream* GetResponse();
		void SetHost(Stream*);
		void SetRequestURL(Stream*);
		void AppendResponse(Stream*);
};
typedef MemList<StoreItem*>* MEMSTOREPTR;
typedef MemList<StoreItem*> MEMSTORE;
class MemStore
{
	private:
		MEMSTOREPTR m_pList;	
		static MemStore* g_pInstance;
	private:
		MemStore();
	public:
		static MemStore* getInstance();
		StoreItem* AppendItem(StoreItem*);
		StoreItem* GetByHostAndUrl(Stream* /*pHost*/, Stream* /*pRequestUrl*/);
};
#endif
