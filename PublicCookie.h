#ifndef __PUBLIC_COOKIE_H__
#define __PUBLIC_COOKIE_H__
#include "Stream.h"
class PublicCookie
{
	public:
		static Stream* getStreamByHost(char*);
};
#endif
