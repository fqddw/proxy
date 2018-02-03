#ifndef __USER_H__
#define __USER_H__
#include "Stream.h"
class User
{
	private:
		Stream* m_pUserName;
		Stream* m_pPassword;
		int m_iId;
		int m_bServing;
		int m_bRecording;
		int m_iIp;
	public:
		User();
		~User();
		static User* LoadByName(Stream*);
		static User* GetUserByAssociatedIp(int);
		void SetUserName(Stream*);
		void SetPassword(Stream*);
		Stream* GetPassword();
		Stream* GetUserName();
		int GetId();
		int IsCapturing(Stream*);
		int IsCapturing(char*);
		int IsRecording();
		int IsServing();
		int LoadServerStatus();
};
#endif
