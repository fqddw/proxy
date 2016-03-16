#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__
class HttpResponseLine
{
	public:
		HttpResponseLine();
		~HttpResponseLine();

		int SetCode(int);
		int GetCode();
		char* GetStatusText();
		int SetStatusText(char*);
		int GetMajorVersion();
		int GetSeniorVersion();
		int SetMajorVersion();
		int SetSeniorVersion();
	private:
		int m_iCode;
		char* m_pStatusText;
		int m_iMajorVersion;
		int m_iSeniorVersion;
};
#endif
