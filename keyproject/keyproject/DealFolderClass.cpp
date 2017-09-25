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
			logError("������־�ļ�\"%s\"ʧ��%u\n",
				GetStringAddress(markFilePathWithName),errorNum);
			break;
		}
		SmartFiler smartFiler;
		uSmartSizeType secSize = 0 ;

		errorNum = smartFiler.openFile(markFilePathWithName,Smart_Write_Flag);
		if (errorNum)
		{
			logError("�򿪱�־�ļ�\"%s\"ʧ��%u\n",
				GetStringAddress(markFilePathWithName),errorNum);
			errorNum += (1 + SmartFiler_ErrorNum_Max_MakeFile);
			break;
		}
		if (false==smartFiler.getNexSection(tailerObj->getByteSize(),false))
		{
			logError("ӳ���־�ļ�\"%s\"ʧ��\n",
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
	//ȡ��
	if (
		false==nameSelector->getNextNumString(
		MyFileInfo::isFolderExits,folderPath)
		)
	{
		logError("�ļ���\"%s\"��ȡ������ʧ��\n",
			GetStringAddress(folderPath));
		return 1;
	}
	//�����ļ���
	if (
		false==MyFileInfo::creatSimpleFolder(
		folderPath+nameSelector->numString)
		)
	{
		logError("�ļ���\"%s\"�´���\"%s\"ʧ��\n",
			GetStringAddress(folderPath),
			GetStringAddress(nameSelector->numString));
		return 2;
	}
	return 0 ;
}


//folderPathName��ʾԭ�ļ��к���
//folderPath·������·�����ܲ��Ǵ����·����
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
		logError("�ļ���\"%s\"���\"%s\"ȡ��ʧ��\n",
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
		logError("�ļ���\"%s\"������\"%s%s\"ʧ��\n",
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
