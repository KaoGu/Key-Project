#include "AnalyticParameter.h"
#include <assert.h>
#include "LogFiler.h"



AnalyticParameter::AnalyticParameter( int argc,char **argv ) :argCount(argc),argList(argv)
{
	int dis = 'a' - 'A' ;
	for (int i = 0 ; i<argCount ; i++)
	{
		char *s = argList[i] ;
		if (s[0]=='-')
		{
			for(int j =1 ; s[j]!='\0' ; j++)
			{
				if ( (s[j]<='Z') && (s[j]>='A') )
				{
					s[j] += dis ;
				}
			}
		}
	}
}


int AnalyticParameter::findArg(const char* para)
{
	int i = 0;
	for(; i<argCount ; i++ )
	{
		if (strcmp(argList[i],para)==0)
		{
			break;
		}
	}
	return i<argCount ? i : -1 ;
}
int AnalyticParameter::findArg(string para)
{
	return findArg(para.c_str());
}
string AnalyticParameter::getArg(int index)
{
	assert((index>=0)&&(index<argCount));
	return argList[index];
}
int AnalyticParameter::getArgValue(string arg)
{
	int index = this->findArg(GetStringAddress(arg))+1;
	if (index&&(index<argCount))
	{
		return atoi(argList[index]);
	}
	else
	{
		return -1 ;
	}
}
int AnalyticParameter::getArgCount()
{
	return argCount ;
}
void AnalyticParameter::showArg()
{
	for (int i = 0 ; i<argCount ; i++)
	{
		logDebug("%s\n",argList[i]);
	}
}
