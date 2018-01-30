#ifndef __PUBLIC_COOKIE_H__
#define __PUBLIC_COOKIE_H__
#include "Stream.h"
class PublicCookie
{
	public:
		static Stream* getStreamByUserIdAndHost(int, char*);
		static int Save(int, char*, char*);
};
#endif
