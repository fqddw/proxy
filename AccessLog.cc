#include "AccessLog.h"
#include "mysql/mysql.h"
#include "string"
#include "iostream"
using namespace std;

AccessLog::AccessLog()
{

}

int AccessLog::Save(char* pHost, char* pRequestString)
{
	mysql_thread_init();
	MYSQL conn;
	mysql_init(&conn);
	mysql_real_connect(&conn, "localhost", "root","123456", "ts", 0, NULL, 0);
	string sql = "INSERT INTO `access_log` VALUES (NULL,'"+string(pHost)+"','"+string(pRequestString)+"')";
	mysql_query(&conn, sql.c_str());
	mysql_close(&conn);
	mysql_thread_end();
	return 0;
}
