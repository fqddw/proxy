#ifndef __ADMINCOMMAND_H__
#define __ADMINCOMMAND_H__

#include "Stream.h"
#include "MemList.h"
#include "AdminModule.h"
using namespace std;
typedef MemList<pair<Stream*,AdminModule*>> ModuleList;
class AdminCommand
{
	public:
		AdminCommand();
		~AdminCommand();
		int LoadModule(char*);
		int UnloadModule(char*);
	private:
		MemList* m_pList;
};

#endif
