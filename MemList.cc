#include "MemList.h"
#include "CommonType.h"
MemNode::MemNode():m_pData(NULL),m_pNext(NULL)
{
}

void* MemNode::GetData()
{
	return m_pData;
}
void MemNode::SetData(void* pData)
{
	m_pData = pData;
}

int MemNode::Append(void* pData)
{
	MemNode* pNode = new MemNode();
	pNode->SetData(pData);
	SetNext(pNode);
	return TRUE;
}

MemList::MemList():m_pHead(NULL),m_pEnd(NULL),cs_(new CriticalSection())
{
}

int MemList::Append(void* pData)
{
	cs_->Enter();
	if(m_pHead == NULL)
	{
		m_pHead = new MemNode();
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
int MemList::Find(void* pData)
{
	cs_->Enter();
	MemNode* pNode = m_pHead;
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

int MemList::Delete(void* pData)
{
	cs_->Enter();
	MemNode* pNode = m_pHead;
	if(pNode == NULL)
	{
		cs_->Leave();
		return FALSE;
	}
	if(pNode->GetData() == pData)
	{
		m_pHead = m_pHead->GetNext();
		delete pNode;
		cs_->Leave();
		return TRUE;
	}
	else
	{
		do
		{
			MemNode* pPreNode = pNode;
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
					cs_->Leave();
					return TRUE;
				}
			}
		}while(pNode);
	}
	cs_->Leave();
	return FALSE;
}

MemNode* MemNode::GetNext()
{
	return m_pNext;
}
int MemNode::SetNext(MemNode* pNode)
{
	m_pNext = pNode;
	return TRUE;
}

int MemList::Show()
{
	MemNode* pNode = m_pHead;
	for(;pNode!=NULL;pNode=pNode->GetNext())
	{
		printf("ListData %d\n",pNode->GetData());
	}
	return TRUE;
}
