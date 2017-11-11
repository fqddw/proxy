#ifndef __AUTHMANAGER_H__
#define __AUTHMANAGER_H__
#include "MemList.h"
#include "Auth.h"

#define NONCE_LENGTH 32
#define OPAQUE_LENGTH 32
#define REALM_STRING (char*)"www.transit-server.com"
class AuthManager
{
	public:
		AuthManager();
		~AuthManager();
		static AuthManager* getInstance();
		Auth* GenerateAuthToken();
		int ExistsNonce(Stream*);
		int ExistsOpaque(Stream*);
		Auth* GetAuthByNonce(Stream*);
		Stream* GetRequireAuthString();
	private:
		MemList<Auth*>* m_pAuthList;
		static AuthManager* m_pInstance;
		CriticalSection* cs_;
};
#endif
