#ifndef __HTTPRESPONSE_LINE_H__
#define __HTTPRESPONSE_LINE_H__
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
		int SetMajorVersion(int);
		int SetSeniorVersion(int);
	private:
		int m_iCode;
		char* m_pStatusText;
		int m_iMajorVersion;
		int m_iSeniorVersion;
};
#endif
