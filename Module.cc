#include "Module.h"
#include "string.h"
#include "CommType.h"

Module::Module(),m_pName(NULL)
{
}
Module::~Module()
{
	if(m_pName)
		delete m_pName;
	m_pName = NULL;
}
int Module::GetName(char* pName, int len)
{
	int nameLen = strlen(m_pName);
	if(nameLen+1 > len)
		return FALSE;
	memcpy(pName, m_pName, nameLen);
	pName[nameLen] = '\0';
	return TRUE;
}

