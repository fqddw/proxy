#ifndef __ADMINCLIENT_H__
#define __ADMINCLIENT_H__

#include "CommonType.h"
#include "TimeLib.h"
#include "IOHandler.h"
#include "InetSocketAddress.h"
#define STATUS_BLOCKING 1
#define STATUS_IDLE 2
#define SOCKET_STATUS_CONNECTING 3
#define SOCKET_STATUS_PRE_CONNECTING 4
#define VERSION 10
#define LENGTH 20
#define CMD 30
#define CMD_GET_AUTH_TOKEN 10001
class AdminClient:public IOHandler
{
	public:
		AdminClient();
		AdminClient(int);
		virtual ~AdminClient();
		int GetState();
		int SetState(int);
		int SetStatusBlocking();
		int SetStatusIdle();
		int OnCanSend();
		int IsIdle();
		int IsConnected();
		Stream* GetSendStream();
		InetSocketAddress* GetAddr();
		virtual int ProccessReceive(Stream*);
		virtual int ProccessSend();
		virtual int ProccessConnectionReset();
		int SetSendStream(Stream*);
		int ProccessConnectionClose();
		void IncUseCount();
		void SetStartTime(struct timespec tCurTime);
		void SetEndTime(struct timespec tCurTime);
	private:
		int m_iContextState;
		int m_iOffset;
		int m_iVersion;
		int m_iLength;
		Stream* m_pPartData;
		int m_isConnected;
		int m_iSocket;
		int m_iState;
		Stream* m_pStream;
		Stream* m_pSendStream;
		int m_bShouldClose;
		int m_iSentTotal;
		int m_iRecvTotal;
		int m_iUseCount;
		struct timespec start_time;
		struct timespec end_time;
};
#endif
