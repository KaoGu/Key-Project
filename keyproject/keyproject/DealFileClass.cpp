#include "DealFileClass.h"
#include "MyFileInfo.h"
#include "SmartFiler.h"
#include "NameSelector.h"
#include "Tailer.h"
#include "DealInfoClass.h"
#include "LogFiler.h"
#include <time.h>
#include <assert.h>


const char *fileHeaderStr = "file_" ;

DealFileClass::DealFileClass( uChar* keys,Tailer* tailerObj,DealInfoClass* infoObj )
	:keys(keys),tailerObj(tailerObj),infoObj(infoObj)
{
}

DealFileClass::~DealFileClass()
{
}

uChar DealFileClass::codeFile( 
	std::string filePathName, 
	std::string oldFileName, 
	std::string desPath,
	NameSelector *desNameSelector )
{
	uChar errorNum = 0 ;

	do
	{
		if (
			false==desNameSelector->getNextNumString(
			MyFileInfo::isFolderExits,desPath)
			)
		{
			errorNum = 1 ;
			logError("文件夹\"%s\"下，针对源\"%s\"取名失败\n",
				GetStringAddress(desPath),
				GetStringAddress(filePathName)
				);
			break;
		}
		desPath = desPath + desNameSelector->numString ;
		SmartFiler srcSmartFiler;
		SmartFiler desSmartFiler;
		errorNum = srcSmartFiler.openFile(filePathName,Smart_Read_Flag) ;
		if (errorNum)
		{
			logError("打开源文件\"%s\"失败:%u\n",
				GetStringAddress(filePathName),errorNum);
			errorNum += 1 ;
			break;
		}
		uSmartSizeType secCount = srcSmartFiler.getFileSectionCount();
		uSmartSizeType secOffset = srcSmartFiler.getFileSectionOffset();

		tailerObj->setInfoSize(oldFileName.length()+File_Header_Length);

		secOffset += tailerObj->getByteSize();	
		if (secOffset&Smart_Section_Cell_High_Mask)
		{
			secCount += (secOffset>>Smart_Section_Cell_Low) ; 
			secOffset = secOffset&Smart_Section_Cell_Low_Mask;
		}

		errorNum = SmartFiler::makeFile(desPath,secCount,secOffset);

		if(errorNum)
		{
			logError("目标文件\"%s\"创建失败%u\n",GetStringAddress(desPath),errorNum);
			errorNum += (2 + SmartFiler_ErrorNum_Max_OpenFile) ;
			break;
		}

		char *s = tailerObj->getInfoAddress();
		strcpy(s,fileHeaderStr);
		memcpy(s+File_Header_Length,GetStringAddress(oldFileName),tailerObj->getInfoSize()-File_Header_Length);
		infoObj->dealInfo2Byte();
		
		errorNum = desSmartFiler.openFile(desPath,Smart_Write_Flag) ;
		if (errorNum)
		{
			logError("打开目标文件\"%s\"失败:%u\n",
				GetStringAddress(desPath),errorNum);
			errorNum += (3+SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_MakeFile);
			break;
		}
		
		if (false==
			desSmartFiler.getNexSection(
					Tailer_Max_Bytes_Size,
					true
			)
			)
		{
			errorNum = 4 + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile  + SmartFiler_ErrorNum_Max_MakeFile;
			logError("映射目标文件\"%s\"开头失败\n",GetStringAddress(desPath));
			break ;
		}
		memcpy(
			desSmartFiler.getSectionAddress(),
			tailerObj->getByteAddress(),
			tailerObj->getByteSize());

		errorNum = this->keyData(
			&srcSmartFiler,0,
			&desSmartFiler,tailerObj->getByteSize()
			);
		if (errorNum)
		{
			errorNum  += (5 + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_MakeFile) ;
		}
	} while (0);

	return errorNum ;
}

bool DealFileClass::renameFile( std::string folderPath,std::string fileName,NameSelector *nameSelector )
{
	if (
		false==nameSelector->getNextNumString(
		MyFileInfo::isFolderExits,folderPath)
		)
	{
		logError("文件夹\"%s\"下\"%s\"取名失败\n",
			GetStringAddress(folderPath),
			GetStringAddress(fileName));
		return false;
	}
	int result = MyFileInfo::reName(
		(folderPath+fileName).data(),
		(folderPath+nameSelector->numString).data());
	if(result==-1)
	{
		logError("文件夹\"%s\"下\"%s\"命名为\"%s\"失败\n",
			GetStringAddress(folderPath),GetStringAddress(fileName),
			GetStringAddress(nameSelector->numString)
			);
		return false;
	}

	return true;
}

uChar DealFileClass::keyData(
	SmartFiler* srcFiler,uSmartSizeType srcIndex,
	SmartFiler* desFiler,uSmartSizeType desIndex )
{
	uChar errorNum = 0 ;
	uSmartSizeType indexRigh = 0 ;
	uSmartSizeType srcSize = srcFiler->getSectionSize();
	uSmartSizeType desSize = desFiler->getSectionSize();

	uChar* srcAddress = srcFiler->getSectionAddress();
	uChar* desAddress = desFiler->getSectionAddress();

	uChar keyIndex = 0 ;
	uChar keyReveseIndex = 0 ;		
	uChar k = 0 ;

	indexRigh = desIndex + keys[keyReveseIndex] + (uSmartSizeType)13;
	if (indexRigh>desSize)
	{
		indexRigh = desSize ;
	}

	do
	{
		if (srcIndex>=srcSize)
		{
			if(false==srcFiler->getNexSection(File_Data_Cell,true))
			{
				if (false==srcFiler->isOver())
				{
					errorNum = 1 ;
				}
				break;
			}
			srcSize = srcFiler->getSectionSize();
			srcAddress = srcFiler->getSectionAddress();
			srcIndex = 0 ;
		}


		if (indexRigh<=desIndex)
		{
			desIndex += ( keys[keyReveseIndex] + (uSmartSizeType)13 );
			if (desIndex>=desSize)
			{
				if(false==dFiler->getNexSection(File_Data_Cell,true))
				{
					if (false==desFiler->isOver())
					{
						errorNum = 2 ;
					}
					break;
				}
				desSize = desFiler->getSectionSize();
				desAddress = desFiler->getSectionAddress();
				desIndex = 0 ;
			}

			++keyReveseIndex;
			indexRigh = desIndex + keys[keyReveseIndex] + (uSmartSizeType)13;
			if (indexRigh>desSize)
			{
				indexRigh = desSize ;
			}
		}

		--indexRigh;

		k = keys[keyIndex]^srcAddress[srcIndex];		
		desAddress[indexRigh] = ~k;

		++keyIndex;
		++srcIndex;
	}while (1);

	return errorNum ;
}

bool DealFileClass::isFileMark()
{
	char* s = tailerObj->getInfoAddress();
	int i = 0 ;
	while(i<File_Header_Length)
	{
		if (fileHeaderStr[i]!=s[i])
		{
			break;
		}
		++i;
	}
	return i>=File_Header_Length;
}

uChar DealFileClass::unCodeFile( SmartFiler * srcSmartFiler , std::string desPath )
{
	uChar errorNum = 0 ;
	do
	{
		uSmartSizeType srcIndex = tailerObj->getByteSize();

		uSmartSizeType srcSecCount = srcSmartFiler->getFileSectionCount();
		uSmartSizeType srcSecOffset = srcSmartFiler->getFileSectionOffset();

		uSmartSizeType desSecCount = srcSecCount ;
		uSmartSizeType desSecOffset = srcIndex;

		//计算
		//desSecCount:desSecOffset = srcSecCount:srcSecOffset - srcIndex
		//去掉头部的高位
		if (desSecOffset&Smart_Section_Cell_High_Mask)
		{
			desSecCount -= (desSecOffset>>Smart_Section_Cell_Low) ;
			desSecOffset &= Smart_Section_Cell_Low_Mask ;
		}
		//去掉头部的偏移
		if (srcSecOffset<desSecOffset)
		{
			assert(desSecCount);
			desSecCount-- ;
			desSecOffset = Smart_Section_Cell - desSecOffset + srcSecOffset  ;
		}
		else
		{
			desSecOffset = srcSecOffset - desSecOffset ;
		}

		
		errorNum = SmartFiler::makeFile(desPath,desSecCount,desSecOffset);
		if(errorNum)
		{
			logError("目标文件\"%s\"创建失败%u\n",GetStringAddress(desPath),errorNum);
			break;
		}

		SmartFiler desSmartFiler;
		errorNum = desSmartFiler.openFile(desPath,Smart_Write_Flag) ;
		if (errorNum)
		{
			logError("打开目标文件\"%s\"失败:%u\n",GetStringAddress(desPath),errorNum);
			errorNum += (1 + SmartFiler_ErrorNum_Max_MakeFile);
			break;
		}

		errorNum = this->unKeyData(
			srcSmartFiler,srcIndex,
			&desSmartFiler,0);

		if (errorNum)
		{
			errorNum += (3+SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_MakeFile);
		}

	} while (0);
	return 0;
}

uChar DealFileClass::unKeyData( 
	SmartFiler* srcFiler,uSmartSizeType srcIndex,
	SmartFiler* desFiler,uSmartSizeType desIndex )
{
	uChar errorNum = 0 ;
	uSmartSizeType indexRigh = 0 ;
	uSmartSizeType srcSize = srcFiler->getSectionSize();
	uSmartSizeType desSize = desFiler->getSectionSize();

	uChar* srcAddress = srcFiler->getSectionAddress();
	uChar* desAddress = desFiler->getSectionAddress();

	uChar keyIndex = 0 ;
	uChar keyReveseIndex = 0 ;		
	uChar k = 0 ;

	indexRigh = srcIndex + keys[keyReveseIndex] + (uSmartSizeType)13;
	if (indexRigh>srcSize)
	{
		indexRigh = srcSize ;
	}

	do
	{
		if (desIndex>=desSize)
		{
			if(false==desFiler->getNexSection(File_Data_Cell,true))
			{
				if (false==desFiler->isOver())
				{
					errorNum = 2 ;
				}
				break;
			}
			desSize = desFiler->getSectionSize();
			desAddress = desFiler->getSectionAddress();
			desIndex = 0 ;
		}


		if (indexRigh<=srcIndex)
		{
			srcIndex += ( keys[keyReveseIndex] + (uSmartSizeType)13 );

			if (srcIndex>=srcSize)
			{
				if(false==srcFiler->getNexSection(File_Data_Cell,true))
				{
					if (false==srcFiler->isOver())
					{
						errorNum = 1 ;
					}
					break;
				}
				srcSize = srcFiler->getSectionSize();
				srcAddress = srcFiler->getSectionAddress();
				srcIndex = 0 ;
			}

			++keyReveseIndex;
			indexRigh = srcIndex + keys[keyReveseIndex] + (uSmartSizeType)13;
			if (indexRigh>srcSize)
			{
				indexRigh = srcSize ;
			}
		}
		--indexRigh;

		k = ~srcAddress[indexRigh];
		desAddress[desIndex] = keys[keyIndex]^k;

		++keyIndex;
		++desIndex;
	}while (1);

	return errorNum ;
}

uChar DealFileClass::checkFile( std::string filePath )
{
	uChar errorNum = 0 ;
	SmartFiler smartFiler;
	do 
	{
		errorNum = smartFiler.openFile(filePath,Smart_Read_Flag);
		if (errorNum)
		{
			break;
		}
		if ((smartFiler.getFileSectionOffset()==0)&&
			(smartFiler.getFileSectionCount()==0))
		{
			break;
		}
		if (false==smartFiler.getNexSection(0x01,true))
		{
			errorNum = 1 + SmartFiler_ErrorNum_Max_OpenFile ;
			break;
		}
	} while (0);
	return errorNum ;
}

uChar DealFileClass::destroyFile( std::string filePath )
{
	srand(time(NULL));
	uChar errorNum = 0 ;


	do 
	{
		uChar* secAddress = NULL ;
		uSmartSizeType secSize = 0 ;
		SmartFiler smartFiler;
		
		errorNum = smartFiler.openFile(filePath,Smart_Write_Flag);
		if (errorNum)
		{
			break;
		}

		if (
			(smartFiler.getFileSectionOffset()==0)
			&&(smartFiler.getFileSectionCount()==0)
			)
		{
			break;
		}

		secSize = 0 ;
		while(smartFiler.getNexSection(File_Data_Cell,true))
		{
			secSize = smartFiler.getSectionSize();
			secAddress = smartFiler.getSectionAddress();
			while(secSize)
			{
				--secSize;
				secAddress[secSize] = rand()%256 ;
			}
		}
			
		if (false==smartFiler.isOver())
		{
			errorNum = 1 + SmartFiler_ErrorNum_Max_OpenFile ;
		}


	} while (0);


	return errorNum ;
}

uChar DealFileClass::checkEqualFile( std::string filePath1,std::string filePath2 )
{
	uChar errorNum = 0 ;

	SmartFiler smartFiler1;
	SmartFiler smartFiler2;

	

	do 
	{
		errorNum = smartFiler1.openFile(filePath1,Smart_Read_Flag);
		if (errorNum)
		{
			errorNum++ ;
			break;
		}

		errorNum = smartFiler2.openFile(filePath2,Smart_Read_Flag);
		if (errorNum)
		{
			errorNum += (1 + SmartFiler_ErrorNum_Max_OpenFile) ;
			break;
		}

		if ((smartFiler1.getFileSectionOffset()!=smartFiler2.getFileSectionOffset())
			||(smartFiler1.getFileSectionCount()!=smartFiler2.getFileSectionCount()))
		{
			errorNum = 1 ;
			break;
		}

		if (
			(smartFiler1.getFileSectionOffset()==0)
			&&(smartFiler1.getFileSectionCount()==0)
			)
		{
			errorNum = 0 ;
			break;
		}

		uChar *secAddress1 = NULL ;
		uChar *secAddress2 = NULL ;
		uSmartSizeType secSize = 0 ;
		
		while(errorNum==0)
		{

			if (false==smartFiler1.getNexSection(File_Data_Cell,true))
			{
				if (false==smartFiler1.isOver())
				{
					errorNum = 2 + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile ;
				}
				break;
			}

			if(false==smartFiler2.getNexSection(File_Data_Cell,true))
			{
				errorNum = 3 + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}

			secSize = smartFiler1.getSectionSize() ;
			if (secSize!=smartFiler2.getSectionSize())
			{
				errorNum = 4 + SmartFiler_ErrorNum_Max_OpenFile + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}

			secAddress1 = smartFiler1.getSectionAddress();
			secAddress2 = smartFiler2.getSectionAddress();

			while(secSize)
			{
				--secSize;
				if (secAddress1[secSize]!=secAddress2[secSize])
				{
					errorNum = 1 ;
					break;
				}
			}

		}

	} while (0);
	return errorNum;
}
