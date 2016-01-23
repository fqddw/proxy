#include "SocketPool.h"

SocketNode::SocketNode()
{
	m_Socket = NULL;
	m_pNext = NULL;
}

SocketNode* SocketNode::GetNext()
{
	return m_pNext;
}
int SocketNode::SetNext(SocketNode* pNode)
{
	m_pNext = pNode;
	return TRUE;
}

Socket* SocketNode::GetSocket()
{
	return m_Socket;
}

int SocketPool::AddSocket(Socket* pSocket)
{
	SocketNode* pNode = new SocketNode(pSocket);
	if(m_pEnd == NULL)
	{
		m_pHeader = pNode;
		m_pEnd = m_pHeader;
	}
	else
	{
		m_pEnd->SetNext(pNode);
		m_pEnd = pNode;
	}
	return TRUE;
}

int SocketPool::GetSocketByAddress(InetSocketAddress* pIpAddress)
{

}
