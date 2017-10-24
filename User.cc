#include "User.h"
#include "string"
#include "string.h"
#include "mysql/mysql.h"

User::User():m_pUserName(NULL),m_pPassword(NULL)
{
}
User* User::LoadByName(Stream* pUserName)
{
	MYSQL* conn = NULL;
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
	User* pUser = new User();
	pUser->SetUserName(pUserName);
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
