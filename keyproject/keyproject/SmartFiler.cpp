#include "SmartFiler.h"
#include "LogFiler.h"
#include "MyFileInfo.h"
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

//firstSectionSize==0表示按默认节的大小来，如果文件小于一节则按文件大小来，
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
			assert("暂不支持类型"==NULL);
			errorNum = 1 ;
			break;
		}

		hFile = CreateFile(filePath.c_str(),createFileModel,0,NULL
			,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			errorNum = 2 ;
			logError("打开文件\"%s\"失败\n",
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
				logError("smartFiler错误码:%l\n",lastError);
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

//保证secsize+Smart_Section_Cell不越界
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
			logError("smartNexSec错误码:%l\n",lastError);
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

uChar SmartFiler::makeFile( std::string filePath,uSmartSizeType secCount,uSmartSizeType secOffset, uChar igError )
{
	uChar errorNum = 0 ;
	do
	{
		FILE* fp = NULL ;
		fopen_s(&fp,filePath.c_str(),"w");
		if (fp==NULL)
		{
			logError("创建文件\"%s\"失败\n",GetStringAddress(filePath));
			errorNum = 1 ;
			break;
		}
		fclose(fp);

		if ((secCount==0)&&(secOffset==0))
		{
			break;
		}

		HANDLE hFile = CreateFile(filePath.data(),GENERIC_WRITE|GENERIC_READ,0,NULL
			,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			logError("创建大文件\"%s\"失败\n",GetStringAddress(filePath));
			errorNum = 2 ;
			break;
		}
		HANDLE hFileMap = CreateFileMapping(hFile,NULL,PAGE_READWRITE,
			secCount>>Smart_Section_Cell_High,
			((secCount<<Smart_Section_Cell_Low) + secOffset),NULL);
		if(hFileMap == NULL)
		{
			DWORD tmpNum = GetLastError();
			if (SmartFiler_ErrorNum_MakeFile_ErrorSize>igError)
			{
				logError("创建大文件时进行映射错误码:%u\n",tmpNum);
			}
			errorNum = SmartFiler_ErrorNum_Max_MakeFile;
			CloseHandle(hFile);
			break;
		}
		CloseHandle(hFileMap);
		CloseHandle(hFile);
	} while (0);
	return errorNum;
}

void showBitArray(uChar *bitArray)
{
	uChar i = bitArrayMaxLen ;

	logDebug("bit:");
	while(i)
	{
		--i;
		logDebug("%d",(bitArray[i]==1));
		if (i%16==0)
		{
			logDebug(",");						
		}
	}
	logDebug("\b \n");						

	logDebug("hex:");
	i = bitArrayMaxLen ; 
	while(i>=4)
	{
		i-=4;
		logDebug("%x",(bitArray[i]+bitArray[i+1]*2+bitArray[i+2]*4+bitArray[i+3]*8));
		if (i%16==0)
		{
			logDebug(",");						
		}
	}
	logDebug("\b \n");

//B K、M、G、T、P、E、
	i = bitArrayMaxLen ;
	while(i&&(bitArray[i-1]==0))
		--i;
	uSmartSizeType sV = 0 ;
	char s[]={"BKMGTPE"};

	logDebug("dec:");
	++i;
	uChar j = 70 ;
	while(j)
	{
		j-=10;
		if (i>j)
		{
			sV = 0 ;
			while(i>j)
			{
				--i;
				sV = (sV<<1) + bitArray[i];
			}
			logDebug("%u%c,",sV,s[j/10]);
		}
	}
	logDebug("\b \n");
}

uChar hexToBit(uChar *hexArray,uChar hexLen,uChar *bitArray,uChar pos)
{
	uChar errorNum = 0 ;
	uChar i = 0 , j = 0 ;

//	showBitArray(bitArray);
/*
	logDebug("pos:%d,hexLen:%d\n",pos,hexLen);
	for( i = 0 ; i< hexLen ; ++i )
	{
		logDebug("%x",hexArray[i]);
	}
	logDebug("\n");
*/

	do 
	{
		i = pos + 4;
		while(hexLen&&(i<=bitArrayMaxLen))
		{
			--hexLen;
			bitArray[i-4] += (0!=(hexArray[hexLen]&(uChar)1));		
			bitArray[i-3] += (0!=(hexArray[hexLen]&(uChar)2));
			bitArray[i-2] += (0!=(hexArray[hexLen]&(uChar)4));
			bitArray[i-1] += (0!=(hexArray[hexLen]&(uChar)8));
			i+=4;
		}
		//	showBitArray(bitArray);
		if (hexLen)
		{
			errorNum = 1 ;
			break;
		}

		i = pos ;
		while (i<bitArrayMaxLen)
		{
			if (bitArray[i]>1)
			{
				bitArray[i+1] += bitArray[i]/2;
				bitArray[i] %= 2 ;
			}
			++i;
		}
		if (bitArray[bitArrayMaxLen])
		{
			errorNum = 2 ;
			break;
		}
	} while (0);
//	showBitArray(bitArray);
	return errorNum;
}

uChar decToBit(uChar *decArray,uChar fracPos,uChar decLen,uChar *bitArray,uChar pos)
{
	uChar errorNum = 0 ;

	uChar i = 0 , j = 0 ,v = 0, pi = 0;
//	showBitArray(bitArray);
/*	
	for(i = 0 ; i<fracPos ; i++)
	{
		logDebug("%d",decArray[i]);		
	}
	logDebug(".");
	for(; i<decLen ; i++)
	{
		logDebug("%d",decArray[i]);		
	}
	logDebug("\n");
*/

	pi = pos ;
	i = 0 ;


	do
	{
		while((i<fracPos)&&(decArray[i]==0))
			i++;
		if (i>=fracPos)
		{
			break;
		}
		if (pi>=bitArrayMaxLen)
		{
			errorNum = 1 ;
			logError("十进制数值过大\n");
			break;
		}
		v = 0 ;
		for(j=i;j<fracPos;j++)
		{
			v = v*10 + decArray[j] ;
			if (v<2)
			{
				decArray[j] = 0 ;
			}
			else
			{
				decArray[j] = v/2 ;
				v = v%2 ;
			}
		}
		bitArray[pi] += v ;
		++pi ;

/*		logDebug("%d,dec:",v);
		for(j=i;j<fracPos;j++)
		{
			logDebug("%d",decArray[j]);
		}
		logDebug("\n");
		*/
	} while (1);

//	showBitArray(bitArray);

	i = decLen;
	pi = pos ;
	do
	{
		while((i>fracPos)&&(decArray[i-1]==0))
			--i;
		if (i<=fracPos)
		{
			break;
		}
		if (pi<=0)
		{
			logWarn("转换成二进制时精度有丢失\n");
			break;
		}
		v = 0 ;
		j = i ;
		while(j>fracPos)
		{
			--j;
			v = v + 2*decArray[j];
			if (v<10)
			{
				decArray[j] = v ;
				v = 0 ;
			}
			else
			{
				decArray[j] = v%10 ;
				v = v/10 ;
			}
//			logDebug("V=%d,",v);
		}
		--pi ;
		bitArray[pi] += v ;
/*		logDebug("%d,dec:",v);
		for(j=fracPos;j<i;j++)
		{
			logDebug("%d",decArray[j]);
		}
		logDebug("\n");
		*/
	} while (1);

	i = pi ;
	while (i<bitArrayMaxLen)
	{
		if (bitArray[i]>1)
		{
			bitArray[i+1] += bitArray[i]/2;
			bitArray[i] %= 2 ;
		}
		++i;
	}
//	showBitArray(bitArray);	
	if (bitArray[bitArrayMaxLen])
	{
		errorNum = 2 ;
	}
	return errorNum;
}


//逗号和空格都是可以做为分隔符进行标记(逗号和空格完全等价)，
//只有加号才表示累加，
//0x表示十六进制，0x开头必须最少表示4个字节，高位0可以省去
//kb简化为K,B可以省略
//如果发现第一个数字串后并未找到加号，则表示结束
//5.5G可以表示为：1、"5.5G" 2、"5G+512M" 3、"" 3、"0X5G+0x200M" 
//4、"5632M" 5、"5767168KB" 6、5905580032B 7、"5,905,580,032"
//8、"0x160000000" 9、"0x1 6000 0000" 10、"0x1,0x60000000"
//注意 "0x500000008"=="0x5,0x8"!="0x5,8"
uChar SmartFiler::stringToValue( std::string str,uSmartSizeType &secCount,uSmartSizeType &secOffset )
{
	uChar errorNum = 0 ;
	uChar bitArray[bitArrayMaxLen+1] = {0} ;
	uChar decArray[decArrayMaxLen+1] = {0} ;
	uChar hexArray[hexArrayMaxLen+1] = {0} ;
	uChar pos = 0 ;
	uChar fracPos = 0 ;

	bool flag = false ;
	bool isDeal = false ;
	std::string::size_type i = 0 , j = 0 , len = str.length();
	uSmartSizeType v = 0 ;
	uSmartSizeType integralPart = 0 ;
	uSmartSizeType fractionalPart = 0 ;
	uSmartSizeType fractionalTimes = 0 ;

	uSmartSizeType GPart = 0 ;
	uSmartSizeType MPart = 0 ;
	uSmartSizeType BPart = 0 ;

	secCount = 0 ;
	secOffset = 0 ;

	logDebug("\nstr = \"%s\"\n",GetStringAddress(str));
//	showBitArray(bitArray);

	do
	{
		i = 0 ;
		for ( i = 0 ; (errorNum==0)&&(i<len) ; i++ )
		{
			if ((str[i]==' ')||(str[i]==','))
			{
				continue;
			}
			if (str[i]=='+')
			{
				continue;
			}
			if ((str[i]>'9')||(str[i]<'0'))
			{
				break;
			}
			if ((str[i]=='0')&&((str[i+1]=='x')||(str[i+1]=='X')))
			{
				j = 0 ;
				for ( i += 2 ; (j<hexArrayMaxLen)&&(i<len) ; ++i )
				{
					if ((str[i]==' ')||(str[i]==','))
					{
						if (((i+2)<len)&&(str[i+1]=='0')&&((str[i+2]=='x')||(str[i+2]=='X')))
						{
							if (j>hexArrayMaxLen_Half)
							{
								logError("续串太长");
								errorNum = 1;
								break;
							}
							if (j==0)
							{
								i+=2;
								continue;
							}
							//处理0-j放到32高位
							errorNum = hexToBit(hexArray,j,bitArray,bitArrayMaxLen_Half) ;
							if (errorNum)
							{
								logError("高位串有问题%d\n",errorNum);
								errorNum += SmartFiler_ErrorNum_Max_StringToValue_Base ;
								j = 0 ;
								break;
							}
							flag = true ;
							i+=2;
							j = 0 ;
						}
						continue;
					}
					if (errorNum)
					{
						break;
					}
					if ((str[i]<='9')&&(str[i]>='0'))
					{
						hexArray[j] = str[i] - '0' ;
						++j;
						continue;
					}
					if ((str[i]<='F')&&(str[i]>='A'))
					{
						hexArray[j] = str[i] - 'A' + 10 ;
						++j;
						continue;
					}
					if ((str[i]<='f')&&(str[i]>='a'))
					{
						hexArray[j] = str[i] - 'a' + 10 ;
						++j;
						continue;
					}
					break;
				}
				if (j)
				{
					switch(str[i])
					{
					case 'g':
					case 'G':
						pos = 30;
						break;
					case 'm':
					case 'M':
						pos = 20;
						break;
					case 'k':
					case 'K':
						pos = 10;
						break;
					default:
						pos = 0 ;
						break;
					}
					errorNum = hexToBit(hexArray,j,bitArray,pos);
					if (errorNum)
					{
						logError("低位串有问题%d\n",errorNum);
						errorNum += (SmartFiler_ErrorNum_Max_StringToValue_Base + SmartFiler_ErrorNum_Max_HexToBit);
						break;
					}
					flag = true ;
				}
				continue;
			}

			decArray[0] = str[i]-'0';
			j = 1 ;
			i++ ;
			for(;i<len;++i)
			{
				if ((str[i]==' ')||(str[i]==','))
				{
					continue;
				}
				if ((str[i]<='9')&&(str[i]>='0'))
				{
					if (j>=hexArrayMaxLen)
					{
						errorNum = 2 ;
						break;
					}
					decArray[j] = str[i] - '0' ;
					++j;
					continue;
				}
				break;
			}
			if (errorNum)
			{
				break;
			}
			fracPos = j ;
			if (str[i]=='.')
			{
				logDebug("小数点%d\n",j);
				i++;
				for(;i<len;++i)
				{
					if ((str[i]==' ')||(str[i]==','))
					{
						continue;
					}
					if ((str[i]<='9')&&(str[i]>='0'))
					{
						if (j>=hexArrayMaxLen)
						{
							errorNum = 3 ;
							break;
						}
						decArray[j] = str[i] - '0' ;
						++j;
						continue;
					}
					break;
				}
				logDebug("小数点%d\n",j);
			}
			if (errorNum)
			{
				break;
			}
			switch(str[i])
			{
			case 'g':
			case 'G':
				pos = 30;
				break;
			case 'm':
			case 'M':
				pos = 20;
				break;
			case 'k':
			case 'K':
				pos = 10;
				break;
			default:
				pos = 0 ;
				break;
			}
			errorNum = decToBit(decArray,fracPos,j,bitArray,pos) ;
			if (errorNum)
			{
				errorNum += (SmartFiler_ErrorNum_Max_StringToValue_Base + SmartFiler_ErrorNum_Max_HexToBit+SmartFiler_ErrorNum_Max_HexToBit);
				break;
			}
			flag = true ;
		}

		if (flag==false)
		{
			errorNum = 4;
			break;
		}
		showBitArray(bitArray);

		i = bitArrayMaxLen ;
		while(i&&(bitArray[i-1]==0))
		{
			--i;
		}
//		logDebug("high,%d\n",i);
		if (i>(bitArrayMaxLen-Smart_Section_Cell_High))
		{
			errorNum = 5;
			break;
		}

		secCount = 0 ;
		while(i>Smart_Section_Cell_Low)
		{
			--i;
			secCount = (secCount<<1) + bitArray[i] ;
		}

		secOffset = 0 ;
		while(i)
		{
			--i;
			secOffset = (secOffset<<1) + bitArray[i] ;						
		}

	} while (0);
	return errorNum;
}
