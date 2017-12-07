#ifndef __MYSQL_H__
#define __MYSQL_H__

#include "mysql/mysql.h"
#include "CommonType.h"
class Mysql
{
	private:
		MYSQL* conn_;
	public:
		Mysql();
		~Mysql();

	public:
		static int GlobalInit();
};
#endif
