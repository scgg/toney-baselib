
#include <stdio.h>
#include <stdlib.h>

#include "argc_opt.h"

#include "testlog.h"
int main(int argc, char *argv[])
{
	printf("test is null \n");
	printf(" test ,do you konw \n");

	testLog();
	// test getopt
	char szOptions[] = ":p:l:";
	Get_Opt cmdOpt;
	cmdOpt.SetOptions(argc, argv, szOptions);
	int nOption;
	while((nOption = cmdOpt.GetOpt()) != EOF)
	{
		switch ( nOption ) 
		{
		case 'p':		//侦听端口	
			{
				int nPort = atoi(cmdOpt.GetCurValue());
				if(nPort == 0)
					printf("port is empty \n");
			}
			break;
		case 'l':		//日志等级
			{
				int nlog_lv = atoi(cmdOpt.GetCurValue());
				printf("start log level %d \n",nlog_lv);
			}
			break;
		default:
			printf("error argv %d \n",nOption);
			break;
		}
	}


	return 0;
}




