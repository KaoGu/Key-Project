#ifndef Tailer_H
#define Tailer_H

#include <string>
#include "GlobleType.h"

#define Tailer_Info_Start_Position 36

//��ΪbytesizeΪ������������,�е�λ����
#define Tailer_Init_Captain_Size 0x0080
#define Tailer_Init_Captain_Size_Low_Mark 0x007f

//���1M
#define Tailer_Max_Bytes_Size 0x00100000
#define Tailer_Max_Info_Size 0x00070000

//��֤TailerInt����4���ֽ�
typedef unsigned int uTailerInt;

class Tailer
{
public:
	Tailer(uChar *keys);
	void setInfoSize(uTailerInt len);
	void dealByte2Info(uChar *fileBytes,uTailerInt fileBytesSize);
	void dealInfo2Byte();
	uChar* getByteAddress()
	{
		return bytes;
	}
	char* getInfoAddress()
	{
		return (char*)(bytes+Tailer_Info_Start_Position);
	}

	uTailerInt getByteSize()
	{
		return byteSize;
	}
	uTailerInt getInfoSize()
	{
		return infoSize;
	}
	~Tailer();
private:
	void resetByteCaptainSize(uTailerInt newByteSize);
	void initPre32Bytes(uChar *arr);
	uChar *keys;
	uChar *bytes;
	uTailerInt byteSize;
	uTailerInt infoSize;
	uTailerInt captainSize;
	uChar pre32Bytes[32];
};

#endif
