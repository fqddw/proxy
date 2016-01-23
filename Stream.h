class Stream
{
	public:
		int GetOffset();
		char* GetData();
	private:
		char* m_pData;
		int m_iLength;
};
