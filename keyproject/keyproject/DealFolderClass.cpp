#include "DealFolderClass.h"
#include "MyFileInfo.h"
#include "SmartFiler.h"
#include "NameSelector.h"
#include "Tailer.h"
#include "DealInfoClass.h"
#include "LogFiler.h"

const char* folderHeaderStr = "folder_";

DealFolderClass::DealFolderClass( uChar* keys,Tailer* tailerObj,DealInfoClass* infoObj )
	:keys(keys),tailerObj(tailerObj),infoObj(infoObj)
{

}

DealFolderClass::~DealFolderClass()
{
}



uChar DealFolderClass::codeFoder( std::string markFilePath,std::string folderName)
{
	uChar errorNum = 0 ;
	do
	{
		tailerObj->setInfoSize(folderName.length()+Folder_Header_Length);
		char *s = tailerObj->getInfoAddress();
		strcpy(s,folderHeaderStr);
		memcpy(s+Folder_Header_Length,GetStringAddress(folderName),tailerObj->getInfoSize()-Folder_Header_Length);
		infoObj->dealInfo2Byte();
	//infoObj->dealByte2Info(tailerObj->getByteAddress(),tailerObj->getByteSize());

//		string markFilePathWithName = markFilePath + "a";
		string markFilePathWithName = markFilePath + "a";
		errorNum = SmartFiler::makeFile(markFilePathWithName,0,tailerObj->getByteSize());
		if (errorNum)
		{
			logError("创建标志文件\"%s\"失败%u\n",
				GetStringAddress(markFilePathWithName),errorNum);
			break;
		}
		SmartFiler smartFiler;
		uSmartSizeType secSize = 0 ;

		errorNum = smartFiler.openFile(markFilePathWithName,Smart_Write_Flag);
		if (errorNum)
		{
			logError("打开标志文件\"%s\"失败%u\n",
				GetStringAddress(markFilePathWithName),errorNum);
			errorNum += (1 + SmartFiler_ErrorNum_Max_MakeFile);
			break;
		}
		if (false==smartFiler.getNexSection(tailerObj->getByteSize(),false))
		{
			logError("映射标志文件\"%s\"失败\n",
				GetStringAddress(markFilePathWithName));
			errorNum = 2 + SmartFiler_ErrorNum_Max_OpenFile  + SmartFiler_ErrorNum_Max_MakeFile;
			break;
		}
		memcpy(smartFiler.getSectionAddress(),tailerObj->getByteAddress(),smartFiler.getSectionSize());
		smartFiler.closeFile();

	} while (0);

	return errorNum;
}

uChar DealFolderClass::creatFolder( std::string folderPath,NameSelector *nameSelector )
{
	//取名
	if (
		false==nameSelector->getNextNumString(
		MyFileInfo::isFolderExits,folderPath)
		)
	{
		logError("文件夹\"%s\"下取任意名失败\n",
			GetStringAddress(folderPath));
		return 1;
	}
	//创建文件夹
	if (
		false==MyFileInfo::creatSimpleFolder(
		folderPath+nameSelector->numString)
		)
	{
		logError("文件夹\"%s\"下创建\"%s\"失败\n",
			GetStringAddress(folderPath),
			GetStringAddress(nameSelector->numString));
		return 2;
	}
	return 0 ;
}


//folderPathName表示原文件夹和名
//folderPath路径，但路径可能不是纯粹的路径，
uChar DealFolderClass::renameFolder(
	std::string folderPathName,
	std::string folderPath,
	NameSelector *nameSelector
	)
{
	if (
		false==nameSelector->getNextNumString(
		MyFileInfo::isFolderExits,folderPath)
		)
	{
		logError("文件夹\"%s\"针对\"%s\"取名失败\n",
			GetStringAddress(folderPathName),
			GetStringAddress(folderPath)
			);
		return 1;
	}
	int result = MyFileInfo::reName(
		folderPathName.data(),
		(folderPath+nameSelector->numString).data());
	if(result==-1)
	{
		logError("文件夹\"%s\"重命名\"%s%s\"失败\n",
			GetStringAddress(folderPathName),
			GetStringAddress(folderPath),
			GetStringAddress(nameSelector->numString));
		return 2;
	}
	return 0;
}

uChar DealFolderClass::renameFolder(
	std::string oldName,
	std::string newName
	)
{
	return (-1==MyFileInfo::reName(oldName.c_str(),newName.c_str()));
}


bool DealFolderClass::isFolderMark()
{
	char* s = tailerObj->getInfoAddress();
	int i = 0 ;
	while(i<Folder_Header_Length)
	{
		if (folderHeaderStr[i]!=s[i])
		{
			break;
		}
		++i;
	}
	return i>=Folder_Header_Length;
}
