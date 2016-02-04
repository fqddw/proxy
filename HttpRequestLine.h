#include "HttpUrl.h"

class HttpRequestLine
{
	public:
		HttpRequestLine();
		char* ToString();
		char* GetMethodString();
		int SetUrl(char*);
		int SetVersion(int,int);
		int Parse();
		int FromString(char*);
		int AppendString(char*,int);
		int GetMethodId(char*);
		HttpUrl* GetUrl();
		int GetMajorVer();
		int GetSeniorVer();
	private:
		int m_iMethod;
		HttpUrl* m_pUrl;
		int m_iMajorVer;
		int m_iSeniorVer;
		char* m_pString;
		int m_iStringLength;
};

class HttpProxyRequestLine
{
};

