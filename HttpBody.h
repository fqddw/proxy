#ifndef __HTTPBODY_H__
#define __HTTPBODY_H__

#include "Stream.h"

#define BODY_TYPE_TRANSFER_ENCODING 1
#define BODY_TYPE_CONTENT_LENGTH 2
#define CS_IN_CHUNK 1
#define CS_IN_BEGIN_CRLF 2
#define CS_IN_LENGTH 3
#define CS_IN_END_CRLF 4
class HttpBody
{
	public:
		HttpBody();
		~HttpBody();
		Stream* ToStream(Stream*);
		int IsEnd(Stream*);
		int IsEnd();
		int SetType(int);
		int GetLastChunkLength();
		int Parse(Stream*);
		int SetContentLength(int);
	private:
		int m_iType;
		int m_iCurLength;
		int m_iLength;
		int m_iChunkState;
		int m_iCurInChunkLength;
		int m_iCurChunkLength;
		int m_iOffset;
		int m_iIsEnd;
		Stream* m_pLengthStream;
};
#endif
