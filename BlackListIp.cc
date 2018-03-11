#include "BlackListIp.h"
#include "mysql/mysql.h"
#include "sstream"
using namespace std;

int IpBlackList::InBlackList(unsigned int ip)
{
	mysql_thread_init();
	MYSQL conn;
	mysql_init(&conn);
	mysql_real_connect(&conn, "localhost", "root","123456", "ts", 0, NULL, 0);
	ostringstream ipstream;
	ipstream<<ip;
	//string sql = "SELECT count(1) FROM `ipblacklist` WHERE `ip`='"+ipstream.str()+string("'");
	string sql = "CALL IpInBlackList("+ipstream.str()+")";
	mysql_query(&conn, sql.c_str());
	MYSQL_RES* res = mysql_use_result(&conn);
	if(!res)
	{
		mysql_close(&conn);
		mysql_thread_end();

		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if(!row)
	{
		mysql_free_result(res);
		mysql_close(&conn);
		mysql_thread_end();

		return 0;
	}
	int bInBlackList = atoi(row[0]);
	mysql_free_result(res);
	mysql_close(&conn);
	mysql_thread_end();
	return bInBlackList;
}
