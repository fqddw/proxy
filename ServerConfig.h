class ServerConfig
{
	public:
		int GetPort();
		void SetPort(int);
	private:
		int m_iPort;
};

class ServerConfigDefault:public ServerConfig
{
	public:
		ServerConfigDefault();
};
