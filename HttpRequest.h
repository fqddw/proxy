#include "Stream.h"
class HttpRequest
{
	public:
		HttpRequest();
		HttpRequest(Stream*);
		int IsHeaderEnd();
	private:
		Stream* m_pStream;
};
