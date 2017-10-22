#ifndef __HTTP_REQUEST_LINE_H__
#define __HTTP_REQUEST_LINE_H__
#include "HttpUrl.h"
#include "HttpRequestMethod.h"
#include "Stream.h"
class HttpRequestLine
{
	public:
		HttpRequestLine();
		~HttpRequestLine();
		char* ToString();
		char* GetMethodString();
		int SetUrl(char*);
		int SetVersion(int,int);
		int Parse();
		int FromString(char*);
		int AppendString(char*,int);
		int GetMethodId(char*);
		int GetMethod();
		HttpUrl* GetUrl();
		int GetMajorVer();
		int GetSeniorVer();
		Stream* GetMethodStream();
	private:
		int m_iMethod;
		HttpUrl* m_pUrl;
		int m_iMajorVer;
		int m_iSeniorVer;
		char* m_pString;
		int m_iStringLength;
		Stream* m_pMethodStream;
};

class HttpProxyRequestLine
{
};
#endif
