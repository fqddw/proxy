class HttpResponseLine
{
	public:
		HttpResponseLine();
		~HttpResponseLine();

		int SetCode(int);
		int GetCode();
		int GetMajorVersion();
		int GetSeniorVersion();
		int SetMajorVersion();
		int SetSeniorVersion();
		int SetResponseText();
		int GetResponseText();
};
