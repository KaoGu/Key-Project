#pragma once

#include "GlobleType.h"
#include <string>
#include <Windows.h>

//16MΪ��λ����ӳ��
#define Smart_Section_Cell 0x00100000
//ռ����24��������λ
#define Smart_Section_Cell_Low 20
#define Smart_Section_Cell_Low_Mask 0x000FFFFF
#define Smart_Section_Cell_High 12
#define Smart_Section_Cell_High_Mask 0xFFF00000

#define Smart_Read_Flag 1
#define Smart_Write_Flag 2

#define SmartFiler_ErrorNum_Max_OpenFile 3
#define SmartFiler_ErrorNum_Max_MakeFile 3
//���ڴ�С�����ʶ���ɵ�ʧ��
#define SmartFiler_ErrorNum_MakeFile_ErrorSize 3

#define SmartFiler_ErrorNum_Max_DecToBit 3
#define SmartFiler_ErrorNum_Max_HexToBit 3

#define SmartFiler_ErrorNum_Max_StringToValue_Base 6



//���16����
#define bitArrayMaxLen 64
#define decArrayMaxLen 45
#define hexArrayMaxLen 16

#define bitArrayMaxLen_Half 32
#define hexArrayMaxLen_Half 8


typedef unsigned int Smart_Flag_Type;


class SmartFiler
{
public:
	SmartFiler();

	void showMsg();

	uChar openFile(std::string filePath,Smart_Flag_Type flag);

	bool getNexSection(uSmartSizeType secSize ,bool isAlign);

	uSmartSizeType getSectionSize()
	{
		return sectionSize;
	}
	uChar* getSectionAddress()
	{
		return (uChar*)pvFile + curSectionOffset;
	}

	uSmartSizeType getFileSectionCount()
	{
		return smartFileSectionCount;
	}
	uSmartSizeType getFileSectionOffset()
	{
		return smartFileSectionOffset ;
	}
	bool moveNexBack(uSmartSizeType secSize);
	bool isOver()
	{
		return (nexSectionNum == smartFileSectionCount)&&
			(nexSectionOffset == smartFileSectionOffset) ;
	}

	bool closeFile();

	static uChar stringToValue(std::string str,uSmartSizeType &secCount,uSmartSizeType &secOffset);
	
	static uChar makeFile(std::string filePath,uSmartSizeType secCount,uSmartSizeType secOffset,uChar igError = 0);

	~SmartFiler();
private:
	HANDLE hFile;
	HANDLE hFileMap;
	PVOID pvFile;

	uSmartSizeType smartFileSectionCount;
	uSmartSizeType smartFileSectionOffset;

	//��ǰӳ��Ľں�
	uSmartSizeType curSectionNum;
	//��ǰӳ��Ľ���ƫ����
	uSmartSizeType curSectionOffset;

	uSmartSizeType nexSectionNum;
	uSmartSizeType nexSectionOffset;

	uSmartSizeType sectionSize;	

	uSmartSizeType createFileModel;
	uSmartSizeType createFileMappingModel;
	uSmartSizeType mapViewOfFileModel;
};

