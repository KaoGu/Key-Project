#pragma once
#include "GlobleType.h"
#include "Tailer.h"

//���Ȳ���㣬ֻ�Ƕ����ݽ��б仯����
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

