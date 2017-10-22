#include "MD5.h"
#include "MD5imp.h"
#include "stdio.h"

Stream* MD5::calc(Stream* pStream)
{
	Stream* pDest = new Stream(16);
	MD5_CTX handle;
	MD5Init(&handle);
	MD5Update(&handle, (unsigned char*)pStream->GetData(), pStream->GetLength());
	MD5Final(&handle, (unsigned char*)pDest->GetData());
	Stream* pHexStr = new Stream(32);
	int i = 0;
	for(i=0;i<16;i++)
		sprintf(pHexStr->GetData()+2*i, "%02x", *(unsigned char*)(pDest->GetData()+i));
	delete pDest;
	return pHexStr;
}
