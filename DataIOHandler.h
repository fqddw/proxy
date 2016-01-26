#ifndef __DATAIOHANDLER_H__
#define __DATAIOHANDLER_H__
#include "IOHandler.h"
#include "Stream.h"
class DataIOHandler:public IOHandler
{
	public:
		DataIOHandler();
		int GetDataStream(Stream**);
};
#endif
