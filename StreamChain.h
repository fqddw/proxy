#define NULL 0 
#define TRUE 1
#define FALSE 0
class StreamNode
{
	public:
		StreamNode();
		int GetLength();
		char* GetData();
		int SetNext(StreamNode*);
	private:
		StreamNode* m_pNext;
		char* m_pString;
		int m_iLength;
};


class StreamChain
{
	public:
		StreamChain();
		int AppendNode(StreamNode*);
	private:
		StreamNode* m_pHead;
		StreamNode* m_pEnd;
};
