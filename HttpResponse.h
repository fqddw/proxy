#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__
#include "CommonType.h"
#include "HttpHeader.h"
#include "HttpBody.h"
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
		private:
				HttpResponseHeader* m_pHeader;
				Stream* m_pStream;
				HttpBody* m_pBody;
};
#endif
