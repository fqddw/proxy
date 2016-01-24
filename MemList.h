#include "CriticalSection.h"
class MemNode
{
	public:
		MemNode();
		void* GetData();
		void SetData(void*);
		int Append(void*);
		MemNode* GetNext();
		int SetNext(MemNode*);
	private:
		void* m_pData;
		MemNode* m_pNext;
};
class MemList
{
	public:
		MemList();
		int Append(void*);
		int Find(void*);
		int Delete(void*);
		int Show();
	private:
		MemNode* m_pHead;
		MemNode* m_pEnd;
		CriticalSection* cs_;
};
