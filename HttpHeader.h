#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__
#include "HttpRequestLine.h"
#include "string"
#include "Stream.h"
#include "MemList.h"
using namespace std;
typedef MemList<pair<string,string> > HttpKeyValueList;
class HttpHeader
{
	public:
		int SetRequestLine(HttpRequestLine*);
		HttpRequestLine* GetRequestLine();
		Stream* ToHeader();
		char* ToProxyHeader();
		int SetUrl(HttpUrl*);
		HttpUrl* GetUrl();
		int GetMethod();
	private:
		HttpRequestLine* m_pRequestLine;
		HttpKeyValueList* m_pKeyValueList;
		HttpUrl* m_pUrl;
};
#endif
