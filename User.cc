#include "User.h"
#include "string"
#include "string.h"
#include "mysql/mysql.h"
#include "CommonType.h"
#include "iostream"
#include "sstream"
using namespace std;

User::User():m_pUserName(NULL),m_pPassword(NULL)
{
}
User* User::LoadByName(Stream* pUserName)
{
	mysql_thread_init();
	MYSQL conn;
	mysql_init(&conn);
	mysql_real_connect(&conn, "localhost", "root","123456", "ts", 0, NULL, 0);
	string name;
	name.append(pUserName->GetData(), pUserName->GetLength());
	string sql = "SELECT * FROM `user` WHERE `name`='"+name+"'";
	mysql_query(&conn, sql.c_str());
	MYSQL_RES* res = mysql_use_result(&conn);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(!row)
	{
		mysql_free_result(res);
		mysql_close(&conn);
		mysql_thread_end();

		return NULL;
	}

	User* pUser = new User();
	pUser->m_iId = atoi(row[0]);
	Stream* pName = new Stream();
	pName->Append(pUserName);
	Stream* pPassword = new Stream();
	pPassword->Append(row[2], strlen(row[2]));
	mysql_free_result(res);
	mysql_close(&conn);
	mysql_thread_end();
	pUser->SetUserName(pName);
	pUser->SetPassword(pPassword);
	return pUser;
}

void User::SetUserName(Stream* pUserName)
{
	m_pUserName = pUserName;
}

void User::SetPassword(Stream* pPassword)
{
	m_pPassword = pPassword;
}


Stream* User::GetUserName()
{
	return m_pUserName;
}


Stream* User::GetPassword()
{
	return m_pPassword;
}


int User::GetId()
{
	return m_iId;
}

User::~User()
{
	if(m_pUserName)
		delete m_pUserName;
	if(m_pPassword)
		delete m_pPassword;
	m_pUserName = NULL;
	m_pPassword = NULL;
}

int User::IsCapturing(char* pUrl)
{
	Stream* pUrlStream = new Stream(pUrl);
	int bCapture = IsCapturing(pUrlStream);
	delete pUrlStream;
	return bCapture;
}
int User::IsCapturing(Stream* pUrl)
{
	mysql_thread_init();
	MYSQL conn;
	mysql_init(&conn);
	mysql_real_connect(&conn, "localhost", "root","123456", "ts", 0, NULL, 0);
	string url;
	url.append(pUrl->GetData(), pUrl->GetLength());
	ostringstream id;
	id<<m_iId;
	string sql = "SELECT `capture` FROM `user_url` WHERE `user_id`='"+id.str()+"' AND `url`='"+url+"'";
	mysql_query(&conn, sql.c_str());
	MYSQL_RES* res = mysql_use_result(&conn);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(!row)
	{
		mysql_free_result(res);
		mysql_close(&conn);
		mysql_thread_end();

		return FALSE;
	}
	int canCapture = atoi(row[0]);
	mysql_free_result(res);
	mysql_close(&conn);
	mysql_thread_end();
	if(canCapture)
		return TRUE;
	return FALSE;

}
