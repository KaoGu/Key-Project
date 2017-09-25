#pragma once
#include "GlobleType.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>


#define Level_Debug 1
#define Level_Out 2
#define Level_File 3
#define Level_Warnning 4
#define Level_Error 5

#define Level_File_Str "---"
#define Level_Warnning_Str "-w-"
#define Level_Error_Str "-e-"

#define GetStringAddress(str)	((str).c_str())

#define LOG_DEBUG

#ifdef LOG_DEBUG
#define logDebug(format,...) printf(format,##__VA_ARGS__)
#else
#define logDebug(format,...)
#endif

#define logOut(format,...) printf(format,##__VA_ARGS__)


#define logFile(format,...)\
{\
	printf(".");\
	do\
	{\
		const char *path = logFiler.getLogPath(); \
		if (*path==NULL)\
		{\
			break;\
		}\
\
		FILE *fp = NULL ;\
		errno_t errnu = fopen_s(&fp,path,"a");\
		if (errnu)\
		{\
			printf("日志文件\"%s\"打开失败:%d\n",path,errnu);\
			break;\
		}\
\
		fprintf( fp ,"%s",Level_File_Str);\
		fprintf( fp , format , ##__VA_ARGS__ );\
\
		errnu = fclose(fp);\
		if(errnu)\
		{\
			printf("关闭日志文件失败\n");\
		}\
	} while (0);\
}


#define logWarn(format,...)\
{\
	printf("w");\
	do\
	{\
		const char *path = logFiler.getLogPath(); \
		if (*path==NULL)\
		{\
			break;\
		}\
		\
		FILE *fp = NULL ;\
		errno_t errnu = fopen_s(&fp,path,"a");\
		if (errnu)\
		{\
			printf("日志文件\"%s\"打开失败:%d\n",path,errnu);\
			break;\
		}\
		\
		fprintf( fp ,"警告：file: \"%s\",lineNum:%d\n%s",__FILE__,__LINE__,Level_Warnning_Str);\
		fprintf( fp , format , ##__VA_ARGS__ );\
		\
		errnu = fclose(fp);\
		if(errnu)\
		{\
			printf("关闭日志文件失败\n");\
		}\
	} while (0);\
}



#define logError(format,...)\
{\
	printf("e");\
	do\
	{\
		const char *path = logFiler.getLogPath(); \
		if (*path==NULL)\
		{\
			break;\
		}\
		\
		FILE *fp = NULL ;\
		errno_t errnu = fopen_s(&fp,path,"a");\
		if (errnu)\
		{\
			printf("日志文件\"%s\"打开失败:%d\n",path,errnu);\
			break;\
		}\
		\
		fprintf( fp ,"错误：file: \"%s\",lineNum:%d\n%s",__FILE__,__LINE__,Level_Error_Str);\
		fprintf( fp , format , ##__VA_ARGS__ );\
		\
		errnu = fclose(fp);\
		if(errnu)\
		{\
			printf("关闭日志文件失败\n");\
		}\
	} while (0);\
}


class LogFiler
{
public:
	LogFiler();

	void resetLogPath(std::string path)
	{
		logPath = path ;
	}

	void _cdecl  Out(int level,char* format,...);

	const char * getLogPath()
	{
		return logPath.c_str();
	}
	
	~LogFiler();
private:
	std::string logPath;
};

extern LogFiler logFiler;
