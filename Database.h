#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "mysql/mysql.h"
#include "string"

class DBResult
{
};
class Database
{
	public:
		Database();
		int Connect();
		void SetUserName(string);
		void SetDbName(string);
		void SetCodeType(string);
		void SetPassword(string);
		int Query(string);
		DBResult* GetResult();
	private:
		MYSQL m_hConn;
		string m_strUserName;
		string m_strPassword;
		string m_strDbName;
		string m_strCodeType;
};
#endif
