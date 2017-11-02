#ifndef __AUTHMANAGER_H__
#define __AUTHMANAGER_H__
#include "MemList.h"
#include "Auth.h"

#define NONCE_LENGTH 32
#define OPAQUE_LENGTH 32
class AuthManager
{
	public:
		AuthManager();
		~AuthManager();
		static AuthManager* getInstance();
		Auth* GenerateAuthToken();
		int ExistsNonce(Stream*);
		int ExistsOpaque(Stream*);
	private:
		MemList<Auth*>* m_pAuthList;
		static AuthManager* m_pInstance;
};
#endif
