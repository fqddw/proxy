class Stream
{
	public:
		Stream();
		~Stream();
		int GetLength();
		char* GetData();
		int Append(char*,int);
	private:
		char* m_pData;
		int m_iLength;
};
