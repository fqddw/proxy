#include "ClientSide.h"
#include "ClientSideTask.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "sys/socket.h"
#include "MemList.h"
extern MemList* pGlobalList;
ClientSide::ClientSide():DataIOHandler(),m_pStream(new Stream())
{
	GetEvent()->SetIOHandler(this);
	m_iState = CLIENT_STATE_IDLE;
}
ClientSide::~ClientSide()
{
	delete m_pStream;
}
ClientSide::ClientSide(int sockfd):DataIOHandler(),m_pStream(new Stream())
{
	m_iState = CLIENT_STATE_IDLE;
	GetEvent()->SetFD(sockfd);
	GetEvent()->SetIOHandler(this);
}

int ClientSide::Proccess()
{
	Stream* pStream = NULL;
	GetDataStream(&pStream);
	if(pStream)
	{
		m_pStream->Append(pStream->GetData(),pStream->GetLength());
		if(!m_pHttpRequest)
		{
			m_pHttpRequest = new HttpRequest(m_pStream);
			if(m_pHttpRequest->IsHeaderEnd())
			{
				int ret = m_pHttpRequest->LoadHttpHeader();
			};
		}
	}
	return TRUE;
}

