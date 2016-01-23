class String
{
	private:
		int m_iLength;
		char* m_pData;
	public:
		String();
		String(char*,int);
		String(char*);
		String(String*);
		int Compare(String*);
};
