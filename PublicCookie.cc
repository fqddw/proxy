#include "PublicCookie.h"
#include "mysql/mysql.h"
#include "string"
#include "CommonType.h"
#include "sstream"
#include "ServerConfig.h"
using namespace std;
extern ServerConfigDefault* g_pServerConfig;

Stream* PublicCookie::getStreamByUserIdAndHost(int userId, char* pHost)
{
	mysql_thread_init();
	MYSQL conn;
	MYSQL* h;
	mysql_init(&conn);
	h = &conn;
	mysql_real_connect(&conn, g_pServerConfig->GetDBHost(), g_pServerConfig->GetDBUsername(),g_pServerConfig->GetDBPassword(), "ts", g_pServerConfig->GetDBPort(), NULL, 0);
	ostringstream os;
	os<<userId;

	mysql_query(h, "SET NAMES utf8");
	mysql_query(h, string(string("SELECT `session_key` FROM `user_session` WHERE `url`='")+string(pHost)+string("' AND `user_id`='"+os.str()+string("'"))).c_str());
	MYSQL_RES* res = mysql_use_result(h);
	MYSQL_ROW row = mysql_fetch_row(res);
	Stream * pStream = NULL;
	if(row)
	{
		pStream = new Stream();
		pStream->Append(row[0]);
	}

	mysql_free_result(res);
	//if(pUser->IsCapturing(m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost()))
	mysql_close(h);
	mysql_thread_end();
	return pStream;
}

int PublicCookie::Save(int iUserId, char* pHost, char* pSession)
{
	mysql_thread_init();
	MYSQL conn;
	MYSQL* h;
	mysql_init(&conn);
	h = &conn;
	mysql_real_connect(&conn, g_pServerConfig->GetDBHost(), g_pServerConfig->GetDBUsername(),g_pServerConfig->GetDBPassword(), "ts", g_pServerConfig->GetDBPort(), NULL, 0);

	mysql_query(h, "SET NAMES utf8");
	//if(pUser->IsCapturing(m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost()))
	std::ostringstream os;
	os<<iUserId;

	mysql_query(h, (string("REPLACE INTO `user_session` SET `user_id`="+os.str()+", `create_time`='0', `url`='")+string(pHost)+string("',`session_key`='")+string(pSession)+string("'")).c_str());
	mysql_close(h);
	mysql_thread_end();
	return TRUE;

}
