#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__
#include "HttpRequestLine.h"
#include "HttpResponseLine.h"
#include "string"
#include "Stream.h"
#include "MemList.h"
using namespace std;
typedef MemList<pair<string,string>* > HttpKeyValueList;
class HttpHeader
{
	public:
		virtual Stream* ToHeader() = 0;
		int SetKeyValueList(HttpKeyValueList*);
	private:
		HttpKeyValueList* m_pKeyValueList;
};

class HttpRequestHeader : public HttpHeader
{
	public:
		HttpUrl* GetUrl();
		int GetMethod();
		Stream* ToHeader();
		int SetRequestLine(HttpRequestLine*);
		HttpRequestLine* GetRequestLine();
		int SetUrl(HttpUrl*);
	private:
		HttpRequestLine* m_pRequestLine;
		HttpUrl* m_pUrl;
};

class HttpResponseHeader : public HttpHeader
{
	public:
		HttpResponseLine* GetResponseLine();
		Stream* ToHeader();
		int SetResponseLine(HttpResponseLine*);
	private:
		HttpResponseLine* m_pHttpResponseLine;
};
#endif
