#define SERVER_CONFIG_FILE_NAME "server.cnf"
class ServerConfig
{
	public:
		int GetPort();
		void SetPort(int);
		int Load();
	private:
		int m_iPort;
};

class ServerConfigDefault:public ServerConfig
{
	public:
		ServerConfigDefault();
};
