class SocketProccessor
{
	public:
		SocketProccessor();
		~SocketProccessor();
	private:
		int Run();
	private:
		int m_iState;
		IOHandler* m_pIOHandler;
};
