#include "StreamChain.h"

StreamChain::StreamChain() : m_pHead(NULL),m_pEnd(NULL)
{
}

int StreamChain::AppendNode(StreamNode* pNode)
{
	if(!m_pEnd)
	{
		m_pHead = pNode;
		m_pEnd = pNode;
		pNode->SetNext(NULL);
	}
	else
	{
		pNode->SetNext(NULL);
		m_pEnd->SetNext(pNode);
		m_pEnd = pNode;
	}
}
StreamNode::StreamNode() : m_pString(NULL),m_iLength(0)
{
}

int StreamNode::GetLength()
{
	return m_iLength;
}

char* StreamNode::GetData()
{
	return m_pString;
}
int StreamNode::SetNext(StreamNode* pNode)
{
	m_pNext = pNode;
	return TRUE;
}

