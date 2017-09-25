#include "GlobleType.h"
#include "ParaData.h"
#include "AnalyticParameter.h"
#include "DealClass.h"
#include "MyFileInfo.h"
#include "SmartFiler.h"
#include "DealFileClass.h"
#include "LogFiler.h"
#include <time.h>
#include <assert.h>
#include <stdio.h>
using namespace std;


int say()
{
	logDebug("say");
	return 1;
}

int main(int argc,char**argv)
{
	logFiler.resetLogPath("log.txt");
	clock_t clockA = clock();
	logOut("运行中");
	do 
	{
		int i = 0 ;

		assert(Smart_Section_Cell>=Tailer_Max_Bytes_Size);

#if IS_TEST
//		logError("%d\n",say());
		logDebug("测试模式.\n");
		uChar c = '*';
//		logDebug("ls\n");
//		logDebug("%s,%d,%c,%d,%u,%d\n","ss",15,c,16,c,c);
//		logFile("%s,%d,%c,%d,%u,%d\n","ss",15,c,16,c,c);
//		logError("39%s,%d,%c,%d,%u,%d\n","ss",15,c,16,c,c);		
//		break;		
		char argList[][50] = {"a.exe","-srcflag","8","-code_model","-Key","kKasldfasdadf",
			"-desPath","F:\\k\\code","-srcPath","F:\\k\\src"};
	//	char argList[][50] = {"a.exe","-srcflag","8","-uncode_model","-Key","kKasldfasdadf",
	//		"-desPath","F:\\k\\uncode","-srcPath","F:\\k\\code"};
	//	char argList[][50] = {"a.exe","-srcflag","8","-code_model","-Key","kKasldfasdadf",
	//		"-desPath","F:\\k\\code","-srcPath","F:\\k\\src"};
//		char argList[][50] = {"a.exe","-srcflag","1","-check_model","-Key","kKasldfasdadf",
//			"-srcPath","F:\\k\\src"};
		argc = sizeof(argList)/sizeof(argList[0]);
//		logDebug("%d\n",argc);
		for( i = 0 ; i<argc ; i++ )
			argv[i] = &argList[i][0];
#endif
		AnalyticParameter analyPara(argc-1,argv+1);
		//	analyPara.showArg();
		ParaData paraData(analyPara);
		if (paraData.modelType==UNDEFINE_MODEL)
		{
			logError("不识别模式\n");
			break;
		}

		DealClass dealClass(&paraData);
		PathPair *p = paraData.pathList.frontNode();
		while(p)
		{
			logFile("%s => %s\n",GetStringAddress(p->srcPath),GetStringAddress(p->desPath));
			dealClass.dealFiles(p);
			p = p->next ;
		}


	} while (0);

	clock_t clockB = clock();
	logOut("总共用时:%u 秒.\n",(clockB-clockA)/CLOCKS_PER_SEC);
	return 0 ;
}