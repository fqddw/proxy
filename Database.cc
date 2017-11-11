#include "Database.h"

Database::Database()
{
	mysql_init(&m_hConn);
}
