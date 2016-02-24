#ifndef __HTTP_HEADER__
#define __HTTP_HEADER__
#include "HttpRequestLine.h"
#include "string"
#include "Stream.h"
using namespace std;
class HttpKeyValueList
{
};
class HttpHeader
{
	public:
		int SetRequestLine(HttpRequestLine*);
		HttpRequestLine* GetRequestLine();
		Stream* ToHeader();
		char* ToProxyHeader();
		int SetUrl(HttpUrl*);
		HttpUrl* GetUrl();
	private:
		HttpRequestLine* m_pRequestLine;
		HttpKeyValueList* m_pKeyValueList;
		HttpUrl* m_pUrl;
};
#endif
