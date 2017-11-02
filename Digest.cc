#include "Digest.h"
#include "CommonType.h"
#include "Stream.h"
#include "memory.h"
#include "string"
#include "mysql/mysql.h"
#define SPACE_BEFORE_KEY_VALUE 1
#define BEFORE_SEPARATOR 2
#define BEFORE_VALUE_SCOPE 3
#define BEFORE_VALUE 4
#define IN_KEY 5 
#define IN_VALUE 6
#define BEFORE_VALUE_SCOPE_END 7

class UserCenter
{
	public:
		static Stream* getPassword(Stream*);
};

Stream* UserCenter::getPassword(Stream* pUserName)
{
	MYSQL* conn;
	conn = mysql_init(NULL);
	mysql_real_connect(conn, "localhost", "root","123456", "ts", 0, NULL, 0);
	std::string name;
	name.append(pUserName->GetData(), pUserName->GetLength());
	std::string sql = "SELECT * FROM `user` WHERE `name`='"+name+"'";
	mysql_query(conn, sql.c_str());
	MYSQL_RES* res = mysql_use_result(conn);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(!row)
		return NULL;

	Stream* pPassword = new Stream();
	pPassword->Append(row[2], strlen(row[2]));
	mysql_free_result(res);
	mysql_close(conn);
	return pPassword;
}
Digest::Digest()
{
}

Digest::Digest(Stream* pStream):
	m_pUserName(NULL),
	m_pNonce(NULL),
	m_pUri(NULL),
	m_pMethod(NULL),
	m_pResponse(NULL),
	m_pRealm(NULL),
	m_pOpaque(NULL),
	m_pQop(NULL),
	m_pNC(NULL),
	m_pCnonce(NULL),
	m_pKeyValueList(NULL)
{
	m_pStream = pStream;
}
Digest::~Digest()
{
	if(m_pStream)
		delete m_pStream;
	m_pStream = NULL;
	if(m_pUserName)
		delete m_pUserName;
	m_pUserName = NULL;
	if(m_pNonce)
		delete m_pNonce;
	m_pNonce = NULL;
	if(m_pUri)
		delete m_pUri;
	m_pUri = NULL;
	if(m_pMethod)
		delete m_pMethod;
	m_pMethod = NULL;
	if(m_pResponse)
		delete m_pResponse;
	m_pResponse = NULL;
	if(m_pRealm)
		delete m_pRealm;
	m_pRealm = NULL;
	if(m_pOpaque)
		delete m_pOpaque;
	m_pOpaque = NULL;
	if(m_pQop)
		delete m_pQop;
	m_pQop = NULL;
	if(m_pNC)
		delete m_pNC;
	m_pNC = NULL;
	if(m_pCnonce)
		delete m_pCnonce;
	m_pCnonce = NULL;
	if(m_pKeyValueList)
		delete m_pKeyValueList;
	m_pKeyValueList = NULL;
}

int Digest::Parse()
{
	KeyValueList* pKeyValueList = new KeyValueList();
	char chDigest[] = "Digest";
	int state = SPACE_BEFORE_KEY_VALUE;
	int offset = 6;
	char* pData = m_pStream->GetData();
	int start = offset;
	int end = start;
	char* pKey,*pValue;
	Stream* pKeyStream;
	Stream* pValueStream;
	int hasScope = FALSE;
	for(;offset< m_pStream->GetLength(); offset++)
	{
		switch(state)
		{
			case SPACE_BEFORE_KEY_VALUE:
				if(pData[offset] != ' ')
				{
					start = offset;
					state = IN_KEY;
				}
				break;
			case IN_KEY:
				if(pData[offset] == '=')
				{
					end = offset;
					pKeyStream = new Stream();
					pKeyStream->Append(pData+start, end - start);
					state = BEFORE_VALUE_SCOPE;

				}
				if(pData[offset] == ' ')
				{
					end = offset;
					pKeyStream = new Stream();
					pKeyStream->Append(pData+start, end - start);
					state = BEFORE_SEPARATOR;
				}
				break;
			case BEFORE_SEPARATOR:
				if(pData[offset] == '=')
				{
					state = BEFORE_VALUE_SCOPE;
				}
				break;
			case BEFORE_VALUE_SCOPE:
				if(pData[offset] != ' ')
				{
					if(pData[offset] == '"')
					{
						hasScope = TRUE;
						state = BEFORE_VALUE;
					}
					else
					{
						hasScope = FALSE;
						start = offset;
						state = IN_VALUE;
					}
				}
				break;
			case BEFORE_VALUE:
				if(pData[offset] != '"')
				{
					start = offset;
					state = IN_VALUE;
					break;
				}
			case IN_VALUE:
				if(hasScope == TRUE)
				{
					if(pData[offset] == '"')
					{
						state = BEFORE_VALUE_SCOPE_END;
						end = offset;
						pValueStream = new Stream();
						pValueStream->Append(pData+start, end - start);
						pKeyValueList->Append(new pair<Stream*, Stream*>(pKeyStream, pValueStream));
					}
				}
				else
				{
					if(offset == m_pStream->GetLength()-1)
					{
						end = offset;
						pValueStream = new Stream();
						pValueStream->Append(pData+start, end - start);
						pKeyValueList->Append(new pair<Stream*, Stream*>(pKeyStream, pValueStream));
					}
					else
					{
						if(pData[offset] == ' ')
						{
							end = offset;
							pValueStream = new Stream();
							pValueStream->Append(pData+start, end - start);
							pKeyValueList->Append(new pair<Stream*, Stream*>(pKeyStream, pValueStream));

							state = BEFORE_VALUE_SCOPE_END;
						}
						if(pData[offset] == ',')
						{
							end = offset;
							pValueStream = new Stream();
							pValueStream->Append(pData+start, end - start);
							
							pKeyValueList->Append(new pair<Stream*, Stream*>(pKeyStream, pValueStream));

							state = SPACE_BEFORE_KEY_VALUE;
						}
					}
				}

				break;
			case BEFORE_VALUE_SCOPE_END:
				if(pData[offset] == ',')
				{
					state = SPACE_BEFORE_KEY_VALUE;
				}
				break;
			default:
				;
		}
	}
	m_pKeyValueList = pKeyValueList;
	Load();
	return TRUE;
}
int Digest::Load()
{
	MemNode<pair<Stream*, Stream*>*>* pNode = m_pKeyValueList->GetHead();
	while(pNode)
	{
		if(pNode->GetData()->first->Equal((char*)"nonce"))
		{
			m_pNonce = new Stream();
			m_pNonce->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"uri"))
		{
			m_pUri = new Stream();
			m_pUri->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"method"))
		{
			m_pMethod = new Stream();
			m_pMethod->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"username"))
		{
			m_pUserName = new Stream();
			m_pUserName->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"cnonce"))
		{
			m_pCnonce = new Stream();
			m_pCnonce->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"nc"))
		{
			m_pNC = new Stream();
			m_pNC->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"opaque"))
		{
			m_pOpaque = new Stream();
			m_pOpaque->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"qop"))
		{
			m_pQop = new Stream();
			m_pQop->Append(pNode->GetData()->second);
		}
		if(pNode->GetData()->first->Equal((char*)"response"))
		{
			m_pResponse = new Stream();
			m_pResponse->Append(pNode->GetData()->second);
		}

		if(pNode->GetData()->first->Equal((char*)"realm"))
		{
			m_pRealm = new Stream();
			m_pRealm->Append(pNode->GetData()->second);
		}


		pNode = pNode->GetNext();
	}

	return TRUE;
}

Stream* Digest::CalcH1()
{
	Stream* pData = new Stream();
	pData->Append(m_pUserName);
	pData->Append((char*)":", 1);
	pData->Append(m_pRealm);
	pData->Append((char*)":", 1);
	Stream* pPassword = UserCenter::getPassword(m_pUserName);
	pData->Append(pPassword);
	delete pPassword;
	Stream* pH1 = MD5::calc(pData);
	delete pData;
	return pH1;
}

Stream* Digest::CalcH2()
{
	Stream* pData = new Stream();
	pData->Append(m_pMethod);
	pData->Append((char*)":", 1);
	pData->Append(m_pUri);
	Stream* pH2 = MD5::calc(pData);
	delete pData;
	return pH2;
}

Stream* Digest::CalcResponse()
{
	Stream* pData = new Stream();
	Stream* pH1 = CalcH1();
	Stream* pH2 = CalcH2();
	pData->Append(pH1);
	delete pH1;
	if(!m_pQop)
	{
		pData->Append((char*)":", 1);
		pData->Append(m_pNonce);
	}
	else
	{
		if(m_pQop->Equal((char*)"auth"))
		{
			pData->Append((char*)":", 1);
			pData->Append(m_pNonce);
			pData->Append((char*)":", 1);
			pData->Append(m_pNC);
			pData->Append((char*)":", 1);
			pData->Append(m_pCnonce);
			pData->Append((char*)":", 1);
			pData->Append(m_pQop);
		}
	}
	pData->Append((char*)":", 1);
	pData->Append(pH2);
	delete pH2;
	Stream* pResponse = MD5::calc(pData);
	delete pData;
	return pResponse;
}

Stream* Digest::GetResponse()
{
	return m_pResponse;
}

Stream* Digest::GetRealm()
{
	return m_pRealm;
}

int Digest::SetMethod(Stream* pMethod)
{
	if(m_pMethod)
		delete m_pMethod;
	m_pMethod = new Stream();
	m_pMethod->Append(pMethod);
	return 0;
}
