#include "Stream.h"
#include "HttpHeader.h"
class HttpRequest
{
	public:
		HttpRequest();
		HttpRequest(Stream*);
		int IsHeaderEnd();
		int LoadHttpHeader();
		HttpHeader* GetHeader();
	private:
		HttpHeader* m_pHttpHeader;
		Stream* m_pStream;
};
