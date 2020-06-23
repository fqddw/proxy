#ifndef __MEM_STORE_H__
#define __MEM_STORE_H__
#include "MemList.h"
#include "Stream.h"
#include "CriticalSection.h"
class StoreItem
{
	private:
		Stream* m_pHost;
		Stream* m_pRequestURL;
		Stream* m_pResponse;
		int m_bSaving;
		CriticalSection* cs_;
	public:
		StoreItem();
		StoreItem(Stream*,Stream*);
		Stream* GetHost();
		Stream* GetRequestURL();
		Stream* GetResponse();
		void SetHost(Stream*);
		void SetRequestURL(Stream*);
		void AppendResponse(Stream*);
		void StartSave();
		int IsSaving();
		void AbortSave();
		void FinishSave();
		void Lock();
		void Unlock();
};
typedef MemList<StoreItem*>* MEMSTOREPTR;
typedef MemList<StoreItem*> MEMSTORE;
class MemStore
{
	private:
		MEMSTOREPTR m_pList;	
		CriticalSection* cs_;
		static MemStore* g_pInstance;
	private:
		MemStore();
	public:
		void Lock();
		void Unlock();
		void Delete(StoreItem*);
		static MemStore* getInstance();
		StoreItem* AppendItem(StoreItem*);
		StoreItem* GetByHostAndUrl(Stream* /*pHost*/, Stream* /*pRequestUrl*/);
};
#endif