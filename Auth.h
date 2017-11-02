#ifndef __AUTH_H__
#define __AUTH_H__
#include "Stream.h"

class Auth
{
	public:
		Auth();
		~Auth();
		void SetNonce(Stream*);
		void SetOpaque(Stream*);
		Stream* GetNonce();
		Stream* GetOpaque();
		Stream* ToStream();
		void SetRealm(char*);
		void SetNonceAuthed(int);
		void SetOpaqueAuthed(int);
	private:
		Stream* m_pNonce;
		int m_bNonceAuthed;
		Stream* m_pOpaque;
		int m_bOpaqueAuthed;
		Stream* m_pRealm;
};
#endif
