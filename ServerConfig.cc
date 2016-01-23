#include "ServerConfig.h"

int ServerConfig::GetPort()
{
	return m_iPort;
}

void ServerConfig::SetPort(int port)
{
	m_iPort = port;
}

ServerConfigDefault::ServerConfigDefault()
{
	SetPort(8000);
}
