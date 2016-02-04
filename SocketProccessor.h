#ifndef __SOCKETPROCCESSOR_H__
#define __SOCKETPROCCESSOR_H__
class IOHandler;
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
#endif
