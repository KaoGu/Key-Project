#include "SmartFiler.h"
#include "LogFiler.h"
#include <assert.h>

SmartFiler::SmartFiler()
	:hFile(INVALID_HANDLE_VALUE),hFileMap(NULL),pvFile(NULL)
{
}

SmartFiler::~SmartFiler(void)
{
	this->closeFile();
}

//firstSectionSize==0��ʾ��Ĭ�ϽڵĴ�С��������ļ�С��һ�����ļ���С����
uChar SmartFiler::openFile(std::string filePath,Smart_Flag_Type flag)
{
	uChar errorNum = 0 ;

	this->closeFile();
	do
	{
		if (flag&Smart_Write_Flag)
		{
			createFileModel = GENERIC_WRITE|GENERIC_READ ;
			createFileMappingModel = PAGE_READWRITE ;
			mapViewOfFileModel = FILE_MAP_WRITE ;			
		}
		else
		if (flag==Smart_Read_Flag)
		{
			createFileModel = GENERIC_READ ;
			createFileMappingModel = PAGE_READONLY ;
			mapViewOfFileModel = FILE_MAP_READ ;			
		}
		else
		{
			assert("�ݲ�֧������"==NULL);
			errorNum = 1 ;
			break;
		}

		hFile = CreateFile(filePath.c_str(),createFileModel,0,NULL
			,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			errorNum = 2 ;
			logError("���ļ�\"%s\"ʧ��\n",
				GetStringAddress(filePath));
			break;
		}
		smartFileSectionCount = 0 ;
		smartFileSectionOffset = GetFileSize(hFile,&smartFileSectionCount);
		
		smartFileSectionCount = (smartFileSectionCount<<Smart_Section_Cell_High) + (smartFileSectionOffset>>Smart_Section_Cell_Low);
		smartFileSectionOffset = smartFileSectionOffset&Smart_Section_Cell_Low_Mask;

		if (smartFileSectionOffset||smartFileSectionCount)
		{
			hFileMap = CreateFileMapping(hFile,NULL,createFileMappingModel,
				smartFileSectionCount>>Smart_Section_Cell_High, 
				(smartFileSectionCount<<Smart_Section_Cell_Low)+smartFileSectionOffset,
				NULL);

			if(hFileMap == NULL)
			{
				CloseHandle(hFile);
				errorNum = 3 ;
				DWORD lastError = GetLastError();
				logError("smartFiler������:%l\n",lastError);
				break;
			}
		}
		curSectionNum = 0 ;
		curSectionOffset = 0 ;
		nexSectionNum = 0 ;
		nexSectionOffset = 0 ;
		sectionSize = 0 ;
	} while (0);
	return errorNum;
}

//��֤secsize+Smart_Section_Cell��Խ��
bool SmartFiler::getNexSection( uSmartSizeType secSize , bool isAlign)
{
	do
	{
		if (pvFile)
		{
			UnmapViewOfFile(pvFile);
			pvFile = NULL;
		}

		if (secSize==0)
		{
//			secSize = Smart_Section_Cell ;
			secSize = Smart_Section_Cell - nexSectionOffset ;
		}
		
		
		uSmartSizeType nexOffset = nexSectionOffset + secSize ;
		uSmartSizeType nexNum = nexSectionNum ;

		if (isAlign)
		{
			if (nexOffset&Smart_Section_Cell_Low_Mask)
			{
				secSize +=  ( Smart_Section_Cell - (nexOffset&Smart_Section_Cell_Low_Mask) ) ;
				nexNum += ( (nexOffset>>Smart_Section_Cell_Low) + 1 );
				nexOffset = 0 ;
			}
		}

		if (nexOffset&Smart_Section_Cell_High_Mask)
		{
			nexNum += (nexOffset>>Smart_Section_Cell_Low) ; 
			nexOffset = nexOffset&Smart_Section_Cell_Low_Mask;
		}

		if ((nexNum>smartFileSectionCount)||((nexNum==smartFileSectionCount)&&(nexOffset>smartFileSectionOffset)))
		{
			if (smartFileSectionCount>nexSectionNum)
			{
				secSize = ( (smartFileSectionCount - nexSectionNum)<<Smart_Section_Cell_Low )
					 + smartFileSectionOffset - nexSectionOffset ;
			}
			else
			{
				secSize = smartFileSectionOffset - nexSectionOffset;
			}
			nexNum = smartFileSectionCount ;
			nexOffset = smartFileSectionOffset ;
		}

		if (secSize==0)
		{
			break;
		}
		pvFile = MapViewOfFile(hFileMap,mapViewOfFileModel,
			nexSectionNum>>Smart_Section_Cell_High,
			(nexSectionNum<<Smart_Section_Cell_Low),
			secSize+nexSectionOffset);
		if(pvFile == NULL)
		{
			DWORD lastError = GetLastError();
			logError("smartNexSec������:%l\n",lastError);
			break;
		}
		curSectionNum = nexSectionNum ;
		curSectionOffset = nexSectionOffset ;

		nexSectionNum = nexNum ;
		nexSectionOffset = nexOffset ;

		sectionSize = secSize;
	} while (0);
	return (pvFile!=NULL);	
}

bool SmartFiler::closeFile()
{
	if (pvFile)
	{
		UnmapViewOfFile(pvFile);
		pvFile = NULL;
	}
	if (hFileMap)
	{
		CloseHandle(hFileMap);
		hFileMap = NULL;
	}
	if (hFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile) ;
		hFile = INVALID_HANDLE_VALUE ;
	}
	return true;
}

void SmartFiler::showMsg()
{

}

bool SmartFiler::moveNexBack( uSmartSizeType secSize )
{
	if (nexSectionOffset>=secSize)
	{
		nexSectionOffset -= secSize;
	}
	else
	{
		uSmartSizeType high = (secSize>>Smart_Section_Cell_Low) ;
		uSmartSizeType nexOff = 0 ;
		secSize &= Smart_Section_Cell_Low_Mask ;
		
		if (nexSectionOffset>=secSize)
		{
			nexOff = nexSectionOffset - secSize;
		}
		else
		{
			nexOff = Smart_Section_Cell - secSize + nexSectionOffset ;
			high++;
		}
		if (nexSectionNum>=high)
		{
			nexSectionNum -= high ;
			nexSectionOffset = nexOff ;
		}
		else
		{
			return false ;
		}
	}
	return true;
}
