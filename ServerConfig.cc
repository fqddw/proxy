#include "ServerConfig.h"
#include "stdio.h"
#include "CommonType.h"
#include "string.h"
#include "stdlib.h"

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

#define BEFORE_KEY 0
#define BEFORE_EQUAL 1
#define BEFORE_VALUE 2
#define BEFORE_CRLF 3
#define IN_KEY 4
#define IN_VALUE 5


int ServerConfig::Load()
{
	FILE* fp = fopen(SERVER_CONFIG_FILE_NAME, "rb");
	if(!fp){
		return FALSE;
	}
	fseek(fp, 0, SEEK_END);
	size_t len  = ftell(fp);
	char* pContent = new char[len+1];
	pContent[len] = '\0';
	fseek(fp, 0 , SEEK_SET);
	size_t count = fread(pContent, len, 1, fp);
	int start = 0;
	int offset = 0;
	int state = BEFORE_KEY;
	char* pKey;
	char* pValue;
	int end = 0;
	while(offset < len) {
		switch(state)
		{
			case BEFORE_KEY:
			if(!(pContent[offset] == ' ' || pContent[offset] == '\t')) {
				state = IN_KEY;
				start = offset;
			}
			break;
			case IN_KEY:
			if(pContent[offset] == ' ' || pContent[offset] == '\t' || pContent[offset] == '=')
			{
				end = offset;
				int keylen = end-start;
				pKey = new char[keylen+1];
				pKey[keylen] = '\0';
				memcpy(pKey, pContent+start, keylen);
				if(pContent[offset] == '=')
					state = BEFORE_VALUE;
				else
					state = BEFORE_EQUAL;
			}
			break;
			case BEFORE_EQUAL:
			if(pContent[offset] == '=')
			{
				state = BEFORE_VALUE;
			}
			break;
			case BEFORE_VALUE:
			if(!(pContent[offset] == ' ' || pContent[offset] == '\t'))
			{
				start = offset;
				state = IN_VALUE;
			}
			break;
			case IN_VALUE:
			if(pContent[offset] == '\t' || pContent[offset] == ' ' || pContent[offset] == '\r' || pContent[offset] == '\n')
			{
				end = offset;
				int vallen = end-start;
				pValue = new char[vallen+1];
				pValue[vallen] = '\0';
				memcpy(pValue, pContent+start, vallen);
				if(strstr(pKey, "db.mysql.host"))
					m_pDBHost = pValue;

				if(strstr(pKey, "db.mysql.user"))
					m_pDBUserName = pValue;
				if(strstr(pKey, "db.mysql.password"))
					m_pDBPassword = pValue;
				if(strstr(pKey, "db.mysql.character"))
					m_pCharacter = pValue;

				if(strstr(pKey, "db.mysql.port"))
				{
					m_iDbPort = atoi(pValue);
					delete pValue;
				}

				delete pKey;
				if(pContent[offset] != '\n')
					state = BEFORE_CRLF;
				else
					state = BEFORE_KEY;
			}
			break;
			case BEFORE_CRLF:
			if(pContent[offset] == '\n')
			{
				state = BEFORE_KEY;
			}
			break;
			default:
			;
		}
		offset++;
	}
	return TRUE;
}
ServerConfigDefault::ServerConfigDefault()
{
	SetPort(8000);
	SetAdminPort(4631);
}

char* ServerConfig::GetDBUsername()
{
	return m_pDBUserName;
}

int ServerConfig::GetDBPort()
{
	return m_iDbPort;
}

char* ServerConfig::GetDBPassword()
{
	return m_pDBPassword;
}

char* ServerConfig::GetDBHost()
{
	return m_pDBHost;
}
