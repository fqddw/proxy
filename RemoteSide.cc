int RemoteSide::Run()
{
	if(m_iState != REMOTE_STATE_IDLE)
	{
		return FALSE;
	}
	else
	{
		m_iState = CLIENT_STATE_RUNNING;
		RemoteSideTask* pTask = new RemoteSideTask();
		pGlobalList->Append(pTask);
		pTask->SetRemoteSide(this);
		GetMasterThread()->InsertTask(pTask);
	}
	return TRUE;
};

int RemoteSide::Proccess()
{
	return TRUE;
}
