#pragma once
#include "GlobleType.h"
#include <string>


#define Folder_Header_Length 7


#define DealFolder_ErrorNum_Max_RenameFolder2Arg 1
#define DealFolder_ErrorNum_Max_RenameFolder3Arg 2
#define DealFolder_ErrorNum_Max_CreatFolder 2
#define DealFolder_ErrorNum_Max_CodeFolder (2 + SmartFiler_ErrorNum_Max_OpenFile)


class NameSelector;
class Tailer; 
class DealInfoClass; 


class DealFolderClass
{
public:
	DealFolderClass(uChar* keys,Tailer* tailerObj,DealInfoClass* infoObj);

	uChar renameFolder(
		std::string folderPathName,
		std::string folderPath,
		NameSelector *nameSelector
		);
	//返回0表示成功
	uChar renameFolder(
		std::string oldName,
		std::string newName
		);
	uChar creatFolder(std::string folderPath,NameSelector *nameSelector);
	uChar codeFoder(std::string markFilePath,std::string folderName);
	bool isFolderMark();


	~DealFolderClass();

	uChar* keys;
	Tailer* tailerObj;
	DealInfoClass* infoObj;
};

