#ifndef __ADMINCOMMAND_H__
#define __ADMINCOMMAND_H__

#include "Stream.h"
#include "MemList.h"
#include "AdminModule.h"
using namespace std;
class ModuleInfo
{
	public:
		void* handle;
		AdminModule* module;
};
typedef MemList<ModuleInfo*> ModuleList;
class AdminCommand
{
	public:
		static AdminCommand* getInstance();
		int LoadModule(char*);
		int UnloadModule(char*);
		Stream* RunComman(int,int,char*);
	private:
		AdminCommand();
		~AdminCommand();
		static ModuleList* g_pList;
};

#endif
