AcceptDispatchTask::AcceptDispatchTask():m_pServer(NULL)
{
}
int AcceptDispatchTask::SetServer(Server* pServer)
{
	m_pServer = pServer;
	return TRUE;
}

AcceptDispatchTask::AcceptDispatchTask(Server* pServer):m_pServer(pServer)
{
}
int AcceptDispatchTask::Run()
{
	struct sockaddr sa = {0};
	socklen_t len = sizeof(sa);
	int client = accept(GetEvent()->GetFD(),&sa,&len);
	char* pResponse = "HTTP/1.1 200 OK\r\nContent-Length: 1\r\n\r\n1";
	send(client,pResponse,strlen(pResponse), 0);
	close(client);
	return TRUE;

}
