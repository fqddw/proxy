#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__
#include "HttpRequestLine.h"
#include "HttpResponseLine.h"
#include "string"
#include "Stream.h"
#include "MemList.h"
using namespace std;

#define HTTP_CONTENT_LENGTH 100
#define HTTP_SERVER 101
#define HTTP_CONNECTION 102
#define HTTP_TRANSFER_ENCODING 103
#define HTTP_COOKIE 104
typedef MemList<pair<string,string>* > HttpKeyValueList;
class HttpHeader
{
	public:
		~HttpHeader();
		virtual Stream* ToHeader() = 0;
		int SetKeyValueList(HttpKeyValueList*);
		HttpKeyValueList* GetKeyValueList();
		char* GetField(int);
		int SetRawLength(int);
		int GetRawLength();
	private:
		int m_iRawLength;
		HttpKeyValueList* m_pKeyValueList;
};

class HttpRequestHeader : public HttpHeader
{
	public:
		~HttpRequestHeader();
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
		~HttpResponseHeader();
		HttpResponseLine* GetResponseLine();
		Stream* ToHeader();
		int SetResponseLine(HttpResponseLine*);
	private:
		HttpResponseLine* m_pHttpResponseLine;
};
#endif
