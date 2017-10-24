#ifndef __USER_SESSION_H__
#define __USER_SESSION_H__

class UserSession
{
	public:
		UserSession();
		~UserSession();
	private:
		Digest* m_pToken;
		User* m_pUser;
};
#endif
