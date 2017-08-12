#include "stdio.h"
#include "Stream.h"

class Digest
{
								public:
																Digest();
																Stream* CalcH1();
																Stream* CalcH2();
																Stream* CalcResponse();
};
