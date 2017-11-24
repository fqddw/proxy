#ifndef __STREAM_H__
#define __STREAM_H__
#include "CriticalSection.h"
class Stream
{
	public:
		Stream();
		Stream(char*);
		Stream(const char*);
		Stream(int);
		~Stream();
		int GetLength();
		char* GetData();
		int Append(char*,int);
		int Append(char*);
		int Append(const char*);
		int Append(Stream*);
		int Sub(int);
		char* GetPartDataToString(int,int);
		Stream* GetPartStream(int,int);
		int Equal(Stream*);
		int Equal(char*);
		void Clear();
		void Lock();
		void Unlock();
	private:
		char* m_pData;
		int m_iLength;
		CriticalSection* cs_;
};
#endif
