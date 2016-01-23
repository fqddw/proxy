class SocketNode
{
	public:
		SocketNode();
		SocketNode* GetNext();
		Socket* GetSocket();
	private:
		Socket* m_pSocket;
		SocketNode* m_pNext;
};
class SocketPool
{
	private:
		SocketPool();
		Socket* GetSocketByAddress(InetSocketAddress*);
		int AddSocket(Socket*);
	private:
		SocketNode* m_pHeader;
		SocketNode* m_pEnd;
};
