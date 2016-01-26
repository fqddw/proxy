#include "CommonType.h"
#include "DataIOHandler.h"
class RemoteSide:public DataIOHandler
{
	public:
		RemoteSide();
		~RemoteSide();
		int GetState();
		int SetState(int);
		int Run();
	private:
		int m_iSocket;
		int m_iState;
		Stream* m_pStream;
		ClientWSide* m_pClientSide;
};

