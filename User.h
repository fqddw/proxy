#ifndef __USER_H__
#define __USER_H__
#include "Stream.h"
class User
{
	private:
		Stream* m_pUserName;
		Stream* m_pPassword;
		int m_iId;
	public:
		User();
		~User();
		static User* LoadByName(Stream*);
		void SetUserName(Stream*);
		void SetPassword(Stream*);
};
#endif
