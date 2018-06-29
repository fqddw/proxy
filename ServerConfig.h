#define SERVER_CONFIG_FILE_NAME "server.cnf"
class ServerConfig
{
	public:
		int GetPort();
		int GetAdminPort();
		char* GetDBHost();
		char* GetDBUsername();
		char* GetDBPassword();
		int GetDBPort();
		void SetPort(int);
		void SetAdminPort(int);
		int Load();
	private:
		char* m_pDBUserName;
		char* m_pDBPassword;
		char* m_pCharacter;
		char* m_pDBHost;
		int m_iDbPort;
		int m_iPort;
		int m_iAdminPort;
};

class ServerConfigDefault:public ServerConfig
{
	public:
		ServerConfigDefault();
};
