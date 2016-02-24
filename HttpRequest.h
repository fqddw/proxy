#include "Stream.h"
#include "HttpHeader.h"
#include "HttpBody.h"
class HttpRequest
{
	public:
		HttpRequest();
		HttpRequest(Stream*);
		int IsHeaderEnd();
		int LoadHttpHeader();
		HttpHeader* GetHeader();
		HttpBody* GetBody();
		int HasBody();
		int LoadBody();
	private:
		HttpHeader* m_pHttpHeader;
		HttpBody* m_pHttpBody;
		Stream* m_pStream;
};
