#ifndef __MEMLIST_H__
#define __MEMLIST_H__
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
		~MemList();
		int Lock();
		int Unlock();

		int Append(T);
		int Find(T);
		int Delete(T);
		int Show();
		MemNode<T>* GetHead();
		MemNode<T>* GetEnd();
	private:
		MemNode<T>* m_pHead;
		MemNode<T>* m_pEnd;
		CriticalSection* cs_;
};
#include "CommonType.h"
template <typename T>
MemNode<T>::MemNode():m_pData(NULL),m_pNext(NULL)
{
}

template<typename T>
T MemNode<T>::GetData()
{
	return m_pData;
}
template<typename T>
void MemNode<T>::SetData(T pData)
{
	m_pData = pData;
}

template<typename T>
int MemNode<T>::Append(T pData)
{
	MemNode<T>* pNode = new MemNode<T>();
	pNode->SetData(pData);
	SetNext(pNode);
	return TRUE;
}

template<typename T>
MemList<T>::MemList():m_pHead(NULL),m_pEnd(NULL),cs_(new CriticalSection())
{
}

template<typename T>
int MemList<T>::Append(T pData)
{
	cs_->Enter();
	if(m_pHead == NULL)
	{
		m_pHead = new MemNode<T>();
		m_pHead->SetData(pData);
		m_pEnd = m_pHead;
	}
	else
	{
		m_pEnd->Append(pData);
		m_pEnd = m_pEnd->GetNext();
	}
	cs_->Leave();
	return TRUE;
}
#include "stdio.h"
template<typename T>
int MemList<T>::Find(T pData)
{
	cs_->Enter();
	MemNode<T>* pNode = m_pHead;
	if(!pNode)
		return FALSE;
	for(;pNode!=NULL;pNode = pNode->GetNext())
	{
		if(pNode->GetData() == pData)
		{
			cs_->Leave();
			return TRUE;
		}
	}
	cs_->Leave();
	return FALSE;
}

template<typename T>
int MemList<T>::Delete(T pData)
{
	cs_->Enter();
	MemNode<T>* pNode = m_pHead;
	if(pNode == NULL)
	{
		cs_->Leave();
		return FALSE;
	}
	if(pNode->GetData() == pData)
	{
		m_pHead = m_pHead->GetNext();
		delete pNode;
		if(pNode == m_pEnd)
			m_pEnd = NULL;
		cs_->Leave();
		return TRUE;
	}
	else
	{
		do
		{
			MemNode<T>* pPreNode = pNode;
			pNode = pNode->GetNext();
			if(pNode != NULL)
			{
				if(pNode->GetData() == pData)
				{
					pPreNode->SetNext(pNode->GetNext());
					if(pNode == m_pEnd)
					{
						m_pEnd = pPreNode;
					}
					delete pNode;
					pNode = pPreNode;
					//cs_->Leave();
					//return TRUE;
				}
			}
		}while(pNode);
	}
	cs_->Leave();
	return FALSE;
}

template<typename T>
MemNode<T>* MemNode<T>::GetNext()
{
	return m_pNext;
}
template<typename T>
int MemNode<T>::SetNext(MemNode<T>* pNode)
{
	m_pNext = pNode;
	return TRUE;
}

template<typename T>
int MemList<T>::Lock()
{
	cs_->Enter();
	return TRUE;
}
template<typename T>
int MemList<T>::Unlock()
{
	cs_->Leave();
	return TRUE;
}
template<typename T>
MemNode<T>* MemList<T>::GetHead()
{
	return m_pHead;
}
template<typename T>
MemNode<T>* MemList<T>::GetEnd()
{
	return m_pEnd;
}

template<typename T>
MemList<T>::~MemList<T>()
{
	MemNode<T>* pNode = m_pHead;
	while(pNode)
	{
		delete pNode->GetData();
		MemNode<T>* pTmpNode = pNode;
		pNode = pNode->GetNext();
		delete pTmpNode;
	}
	delete cs_;
	cs_ = NULL;
}
#endif
