#pragma once
#include "GlobleType.h"
#include <string>

class NameSelector;
class Tailer;
class DealInfoClass;

#define DealFile_ErrorNum_Max_KeyData 2
#define DealFile_ErrorNum_Max_UnKeyData 2
#define DealFile_ErrorNum_Max_CodeFile (5 + DealFile_ErrorNum_Max_KeyData + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_MakeFile)
#define DealFile_ErrorNum_Max_UnCodeFile (3 + DealFile_ErrorNum_Max_UnKeyData + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_MakeFile)
#define DealFile_ErrorNum_Max_CheckFile (1 + SmartFiler_ErrorNum_Max_OpenFile)
#define DealFile_ErrorNum_Max_DestroyFile (1 + SmartFiler_ErrorNum_Max_OpenFile)

#define DealFile_ErrorNum_Max_CheckEqualFile (4 + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile )

#define File_Header_Length 5
#define File_Data_Cell 0x04000000

class SmartFiler;

class DealFileClass
{
public:
	DealFileClass(uChar* keys,Tailer* tailerObj,DealInfoClass* infoObj);
	bool renameFile(std::string filePath,std::string folderName,NameSelector *nameSelector);
	uChar codeFile(
		std::string filePathName,
		std::string oldFileName,
		std::string desPath,
		NameSelector *desNameSelector
		);
	uChar checkFile(std::string filePath);
	uChar destroyFile(std::string filePath);
	uChar unCodeFile(SmartFiler * srcFiler , std::string desPath);
	uChar keyData(
		SmartFiler* srcFiler,uSmartSizeType srcIndex,
		SmartFiler* desFiler,uSmartSizeType desIndex);
	uChar unKeyData(
		SmartFiler* srcFiler,uSmartSizeType srcIndex,
		SmartFiler* desFiler,uSmartSizeType desIndex);
	bool isFileMark();

	//0表示相同，1表示不同，其它表示错误
	static uChar checkEqualFile(std::string filePath1,std::string filePath2);

	~DealFileClass();

	uChar* keys;
	Tailer* tailerObj;
	DealInfoClass* infoObj;
};

