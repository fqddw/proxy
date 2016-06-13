#include "Stream.h"
#include "HttpHeader.h"
#include "HttpBody.h"
class HttpRequest
{
	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(Stream*);
		int IsHeaderEnd();
		int LoadHttpHeader();
		HttpRequestHeader* GetHeader();
		HttpBody* GetBody();
		int HasBody();
		int LoadBody();
		int GetAuthStatus();
	private:
		HttpRequestHeader* m_pHttpHeader;
		HttpBody* m_pHttpBody;
		Stream* m_pStream;
};
