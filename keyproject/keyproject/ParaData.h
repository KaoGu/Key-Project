#pragma once
#ifndef ParaData_H
#define ParaData_H

#include "GlobleType.h"
#include "MyList.h"
#include <Windows.h>

class AnalyticParameter;

class ParaData{
public:

	ParaData()
		:modelType(UNDEFINE_MODEL),
		srcFlag(All_False_Flag)
	{
	}

	void showHelp();

	void findModel(AnalyticParameter &analyPara);


	MODEL_TYPE modelType;

	uChar key[256];

		

	//ÐÂ³ÉÔ±
	Contain_Flag_TYPE srcFlag;

	TypeClass<PathPair>::List pathList;

private:
	void initByteKey(uChar key[256]);
};

#endif