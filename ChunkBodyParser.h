#include "unistd.h"

class ChunkBodyParser
{
	public:
		int GetOffset();
		int GetState();
		int AppendString();
		int IsEnd();
		int ParseString();
	private:
		StreamChain* m_Content;
};
