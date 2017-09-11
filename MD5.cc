#include "MD5.h"
#include "MD5imp.h"

Stream* MD5::calc(Stream* pStream)
{
	Stream* pDest = new Stream(16);
	MD5_CTX handle;
	MD5Init(&handle);
	MD5Update(&handle, (unsigned char*)pStream->GetData(), pStream->GetLength());
	MD5Final(&handle, (unsigned char*)pDest->GetData());
	return pDest;
}
