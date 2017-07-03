#ifndef __STREAM_H__
#define __STREAM_H__
#include "CriticalSection.h"
class Stream
{
	public:
		Stream();
		Stream(int);
		~Stream();
		int GetLength();
		char* GetData();
		int Append(char*,int);
		int Sub(int);
		char* GetPartDataToString(int,int);
		Stream* GetPartStream(int,int);
	private:
		char* m_pData;
		int m_iLength;
		CriticalSection* cs_;
};
#endif
