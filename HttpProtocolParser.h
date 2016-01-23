class HttpProtocolParser
{
	public:
		int AppendContent(char*);
		int Parse();
	private:
		int m_iState;
		int m_iOffset;
		int m_iLength;
		char* m_pContent;
		HttpHeader* m_pHeader;
		HttpBody* m_pBody;
};

