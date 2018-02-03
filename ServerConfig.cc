#include "ServerConfig.h"
#include "stdio.h"
#include "CommonType.h"

int ServerConfig::GetPort()
{
	return m_iPort;
}
int ServerConfig::GetAdminPort()
{
	return m_iAdminPort;
}

void ServerConfig::SetPort(int port)
{
	m_iPort = port;
}
void ServerConfig::SetAdminPort(int port)
{
	m_iAdminPort = port;
}


int ServerConfig::Load()
{
				FILE* fp = fopen(SERVER_CONFIG_FILE_NAME, "r");
				return TRUE;
}
ServerConfigDefault::ServerConfigDefault()
{
	SetPort(8000);
	SetAdminPort(4631);
}
