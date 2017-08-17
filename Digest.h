#include "stdio.h"
#include "Stream.h"

class Digest
{
	public:
		Digest();
		Digest(Stream*);
		Stream* CalcH1();
		Stream* CalcH2();
		Stream* CalcResponse();
		int Parse();
	private:
		Stream* m_pStream;
};
