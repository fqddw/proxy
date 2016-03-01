#ifndef __STREAM_H__
#define __STREAM_H__
class Stream
{
	public:
		Stream();
		~Stream();
		int GetLength();
		char* GetData();
		int Append(char*,int);
		int Sub(int);
	private:
		char* m_pData;
		int m_iLength;
};
#endif
