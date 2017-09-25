#pragma once
#include "GlobleType.h"
#include "Tailer.h"

//长度不变层，只是对数据进行变化处理
class DealInfoClass
{
public:
	DealInfoClass(uChar* keys,Tailer* tailer);
	~DealInfoClass();

	void dealByte2Info(uChar *fileBytes,uTailerInt fileBytesSize);
	void dealInfo2Byte();

	uChar* keys;
	Tailer* tailerObj;
};

