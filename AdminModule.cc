#include "AdminModule.h"

AdminModule::~AdminModule()
{
}

Stream* AdminModule::GetName()
{
	return new Stream("AdminBase");
}

AdminModule::AdminModule()
{
}

Stream* AdminModule::RunCommand(int version, int length, Stream* pCommand)
{
	return NULL;
}
