#pragma once
#ifndef ParaData_H
#define ParaData_H

#include "GlobleType.h"
#include "MyList.h"
#include <Windows.h>

class AnalyticParameter;

class ParaData{
public:

	ParaData(AnalyticParameter &analyPara);

	MODEL_TYPE modelType;

	uChar key[256];

	SHOW_TYPE showType;
		

	//ÐÂ³ÉÔ±
	Contain_Flag_TYPE srcFlag;

	TypeClass<PathPair>::List pathList;

private:
	void showHelp();
	void initByteKey(uChar key[256]);
};

#endif