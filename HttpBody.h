#ifndef __HTTPBODY_H__
#define __HTTPBODY_H__

#include "Stream.h"
class HttpBody
{
		public:
				Stream* ToStream(Stream*);
				int IsEnd();
};
#endif
