#ifndef AnalyticParameter_H
#define AnalyticParameter_H
#include <string>
using namespace std;

class AnalyticParameter
{
public:
	AnalyticParameter(int argc,char **argv);
	//���Ҵ����Ҳ�������-1
	int findArg(const char* para);
	int findArg(string para);
	string getArg(int index);
	int getArgValue(string arg);
	int getArgCount();
	void showArg();
private:
	int argCount;
	char **argList;
};



#endif
