#include "CriticalSection.h"
template<typename T> class MemNode
{
	public:
		MemNode();
		T GetData();
		void SetData(T);
		int Append(T);
		MemNode<T>* GetNext();
		int SetNext(MemNode*);
	private:
		T m_pData;
		MemNode<T>* m_pNext;
};
template<typename T> class MemList
{
	public:
		MemList();
		int Lock();
		int Unlock();

		int Append(T);
		int Find(T);
		int Delete(T);
		int Show();
	private:
		MemNode<T>* m_pHead;
		MemNode<T>* m_pEnd;
		CriticalSection* cs_;
};
