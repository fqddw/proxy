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
	private:
		int m_iMethod;
		char* m_pUrl;
		int m_iMajorVer;
		int m_iSeniorVer;
		char* m_pString;
		int m_iStringLength;
};

class HttpProxyRequestLine
{
};

