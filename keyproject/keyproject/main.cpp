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
	do 
	{
		uChar errorNum = 0 ;
		int index = 0 ;

		assert(Smart_Section_Cell>=Tailer_Max_Bytes_Size);

#if IS_TEST
//		logError("%d\n",say());
		logDebug("����ģʽ.\n");
		uChar c = '*';
//		logDebug("ls\n");
//		logDebug("%s,%d,%c,%d,%u,%d\n","ss",15,c,16,c,c);
//		logFile("%s,%d,%c,%d,%u,%d\n","ss",15,c,16,c,c);
//		logError("39%s,%d,%c,%d,%u,%d\n","ss",15,c,16,c,c);		
//		break;		
//		char argList[][100] = {"a.exe","-code_model","-chck","-logPath","F:\\k\\log.txt","-Key","kKasldfasdadf",
//			"-desPath","F:\\k\\code","-srcPath","F:\\k\\src"};
//		char argList[][100] = {"a.exe","-uncode_model","-chek","-logPath","F:\\k\\log.txt","-Key","kKasldfasdadf",
//			"-desPath","F:\\k\\uncode","-srcPath","F:\\k\\code"};
//		char argList[][100] = {"a.exe","-destroy_model","-chek","-Key","kKasldfasdadf",
//			"-srcPath","F:\\k\\uncode"};

//		char argList[][100] = {"a.exe","-logPath","F:\\k\\log.txt","-fill_model","-chek","-Key","kKasldfasdadf",
//			"-srcPath","E:\\qq\\fillfolder"};

//		char argList[][100] = {"a.exe","-logPath","F:\\k\\log.txt","-mkfile","F:\\k\\bigfile","0XF2345678,0xf5671234"};
//		char argList[][100] = {"a.exe","-logPath","F:\\k\\log.txt","-mkfile","D:\\qq\\bigfile","66.4G+43M"};
//		71296457113.6
//		555679647531.008
		argc = sizeof(argList)/sizeof(argList[0]);
//		logDebug("%d\n",argc);
		for( index = 0 ; index<argc ; index++ )
			argv[index] = &argList[index][0];
#endif
		AnalyticParameter analyPara(argc-1,argv+1);
		//	analyPara.showArg();
		index = analyPara.findArg("-logpath") + 1 ;
		if ((index>0)&&(index<analyPara.getArgCount()))
		{
			std::string tmpStr = analyPara.getArg(index);
			logFiler.resetLogPath(tmpStr);
		}
		logFile("<--------------����ʼ-------------->---\n");

		ParaData paraData;
		if (analyPara.findArg("-help")>=0)
		{
			logFile("ģʽ-����\n");
			paraData.showHelp();
			break;
		}
		paraData.findModel(analyPara);

		if (paraData.modelType!=UNDEFINE_MODEL)
		{
			logFile("ģʽ-�ļ�����\n");
			DealClass dealClass(&paraData);
			PathPair *p = paraData.pathList.frontNode();
			while(p)
			{
				logFile("%s => %s\n",GetStringAddress(p->srcPath),GetStringAddress(p->desPath));
				errorNum = dealClass.dealFiles(p);
				if (errorNum)
				{
					logError("�ļ������쳣%d\n",errorNum);
				}
				p = p->next ;
			}
			break;
		}

		index = analyPara.findArg("-mkfile") ;
		if ((index>=0)&&((index+2)<analyPara.getArgCount()))
		{
			logFile("ģʽ-�����ļ�\n");
			std::string filePath = analyPara.getArg(index+1);
			uSmartSizeType secCount = 0 ;
			uSmartSizeType secOffset = 0 ;
			errorNum = SmartFiler::stringToValue(analyPara.getArg(index+2),secCount,secOffset);
			if (errorNum)
			{
				logError("�����ļ���Сʧ��\n");
				break;
			}
			uChar errorNum = SmartFiler::makeFile(filePath,secCount,secOffset);
			if (errorNum)
			{
				logError("�����ļ�\"%s\"��СΪ%u,%u��ʧ��%u\n",
					GetStringAddress(filePath),secCount,secOffset,errorNum);
			}
			else
			{
				logFile("�ɹ������ļ�\"%s\"��СΪ%u,%u\n",
					GetStringAddress(filePath),secCount,secOffset);
			}
            break;
		}
		logError("��ʶ��ģʽ\n");
	} while (0);
	logFile("<--------------�������-------------->---\n");
	clock_t clockB = clock();
	logOut("�ܹ���ʱ:%u ��.\n",(clockB-clockA)/CLOCKS_PER_SEC);
	return 0 ;
}
