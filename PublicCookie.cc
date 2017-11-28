#include "PublicCookie.h"
#include "mysql/mysql.h"
#include "string"
#include "CommonType.h"
#include "sstream"
using namespace std;

Stream* PublicCookie::getStreamByHost(char* pHost)
{
	MYSQL conn;
	MYSQL* h;
	mysql_init(&conn);
	h = &conn;
	mysql_real_connect(h, "localhost", "root", "123456", "ts", 0, NULL, 0);

	mysql_query(h, "SET NAMES utf8");
	mysql_query(h, string(string("SELECT `session_key` FROM `user_session` WHERE `url`='")+string(pHost)+string("'")).c_str());
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
}

int PublicCookie::Save(int iUserId, char* pHost, char* pSession)
{
	MYSQL conn;
	MYSQL* h;
	mysql_init(&conn);
	h = &conn;
	mysql_real_connect(h, "localhost", "root", "123456", "ts", 0, NULL, 0);

	mysql_query(h, "SET NAMES utf8");
	//if(pUser->IsCapturing(m_pHttpRequest->GetHeader()->GetRequestLine()->GetUrl()->GetHost()))
	std::ostringstream os;
	os<<iUserId;

	mysql_query(h, (string("REPLACE INTO `user_session` SET `user_id`="+os.str()+", `create_time`='0', `url`='")+string(pHost)+string("',`session_key`='")+string(pSession)+string("'")).c_str());
	mysql_close(h);
	return TRUE;

}
