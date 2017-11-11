#include "stdio.h"
#include "Stream.h"
#include "MD5.h"
#include "MemList.h"
#include <utility>
using namespace std;
typedef MemList<pair<Stream*,Stream*>* > KeyValueList;
class Digest
{
	public:
		Digest();
		Digest(Stream*);
		~Digest();
		Stream* CalcH1();
		Stream* CalcH2();
		Stream* CalcResponse();
		Stream* GetResponse();
		Stream* GetRealm();
		Stream* GetNonce();
		Stream* GetUserName();
		int SetMethod(Stream*);
		int SetPassword(Stream*);
		int Parse();
		int Load();
	private:
		Stream* m_pStream;
		Stream* m_pUserName;
		Stream* m_pRealm;
		Stream* m_pNonce;
		Stream* m_pUri;
		Stream* m_pMethod;
		Stream* m_pQop;
		Stream* m_pResponse;
		Stream* m_pCnonce;
		Stream* m_pNC;
		Stream* m_pOpaque;
		Stream* m_pPassword;
		KeyValueList* m_pKeyValueList;
};
