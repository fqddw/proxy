#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__
#include "CommonType.h"
#include "HttpHeader.h"
#include "HttpBody.h"

#define HEADER_NOTFOUND 0
#define HEADER_FOUND 1

class HttpResponse
{
		public:
				HttpResponse();
				HttpResponse(Stream*);
				int IsHeaderEnd();
				HttpResponseHeader* GetHeader();
				int LoadHttpHeader();
				int LoadBody();
				HttpBody* GetBody();
				int HasBody();
				int SetState(int);
				int GetState();
		private:
				HttpResponseHeader* m_pHeader;
				Stream* m_pStream;
				HttpBody* m_pBody;
				int m_iState;
};
#endif
