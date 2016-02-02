#ifndef __HTTP_HEADER__
#define __HTTP_HEADER__
#include "HttpRequestLine.h"
#include "string"
using namespace std;
class HttpKeyValueList
{
};
class HttpHeader
{
	public:
		int SetRequestLine(HttpRequestLine*);
		HttpRequestLine* GetRequestLine();
	private:
		HttpRequestLine* m_pRequestLine;
		HttpKeyValueList* m_pKeyValueList;
		string m_pUrl;
};
#endif
