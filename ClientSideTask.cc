#include "ClientSideTask.h"

ClientSideTask::ClientSideTask()
{
}
ClientSideTask::ClientSideTask(ClientSide*)
{
}
int ClientSideTask::SetClientSide(ClientSide* pClientSide)
{
	m_pClientSide = pClientSide;
	return FALSE;
}
int ClientSideTask::Run()
{
	return m_pClientSide->Proccess();
}
