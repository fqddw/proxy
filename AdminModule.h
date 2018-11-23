#ifndef __ADMIN_MODULE_H__
#define __ADMIN_MODULE_H__

#include "Stream.h"
class AdminModule
{
	public:
		AdminModule();
		~AdminModule();
		virtual Stream* RunCommand(int,int,Stream*);
		virtual Stream* GetName();
};

#endif
