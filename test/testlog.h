
#include "log.h"

using namespace comm::log;

void testLog()
{
	printf("test clog  \n");
	comm::log::CLog log;
	log.LOG_OPEN(LOG_TRACE,LOG_TYPE_CYCLE,"./","log_",DEFAULT_MAX_FILE_SIZE,DEFAULT_MAX_FILE_NO);
	log.LOG_P_ALL(LOG_ERROR,"test the clog error out to file \n");

	
}