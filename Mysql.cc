#include "Mysql.h"

int Mysql::GlobalInit()
{
	mysql_library_init(0, NULL, NULL);
	return TRUE;
}
