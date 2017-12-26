#define SERVER_CONFIG_FILE_NAME "server.cnf"
class ServerConfig
{
	public:
		int GetPort();
		int GetAdminPort();
		void SetPort(int);
		void SetAdminPort(int);
		int Load();
	private:
		int m_iPort;
		int m_iAdminPort;
};

class ServerConfigDefault:public ServerConfig
{
	public:
		ServerConfigDefault();
};
