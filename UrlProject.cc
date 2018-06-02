#include "UrlProject.h"
#include "ServerConfig.h"

extern ServerConfigDefault* g_pServerConfig;
int UrlProject::GetIpIntFromUidAndUrl(int id, Stream** pUrl)
{
	mysql_thread_init();
	MYSQL conn;
	mysql_init(&conn);
	mysql_real_connect(&conn, g_pServerConfig->GetDBHost(), g_pServerConfig->GetDBUsername(),g_pServerConfig->GetDBPassword(), "ts", g_pServerConfig->GetDBPort(), NULL, 0);
	string url;
	url.append(pUrl->GetData(), pUrl->GetLength());
	string sql = "CALL GetCustomUrl("+strId+","+url+")";
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

	return pUser;
}
