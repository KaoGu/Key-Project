#include "DealClass.h"
#include <Windows.h>
#include "MyFileInfo.h"
#include "DealFolderClass.h"
#include "SmartFiler.h"
#include "LogFiler.h"
#include <assert.h>
#include <io.h>
#include <cstdlib>
using namespace std;


DealClass::DealClass( ParaData *paraData ) :paraData(paraData),
	tailerObj(paraData->key),
	dealInfoObj(paraData->key,&tailerObj),
	dealFileObj(paraData->key,&tailerObj,&dealInfoObj),
	dealFolderObj(paraData->key,&tailerObj,&dealInfoObj),
	isDestroyFile(paraData->srcFlag&Destroy_File_Flag),
	isRenameFile(paraData->srcFlag&Rename_File_Flag),
	isRenameFolder(paraData->srcFlag&Rename_Folder_Flag),
	isToNewPath(paraData->srcFlag&To_New_Path_Flag)
{
}

void DealClass::test(std::string title,bool isOut)
{
#if Life_Tag
	PathStruct* endP = NULL;
	PathStruct* tagP = pathList.frontNode();
	while(tagP)
	{
		if (isOut)
		{
			logDebug("%s>%s\n",GetStringAddress(title),GetStringAddress(tagP->bufFirst));
		}
		if (tagP==testPointer)
		{
			logDebug("%spathlist中发现testpointer\n",GetStringAddress(title));
		}
		if (tagP->lifeTag)
		{
			logDebug("%s%s\n",GetStringAddress(title),GetStringAddress(tagP->bufFirst));
			assert("0048错误发现"==NULL);
		}
		tagP->lifeTag = true;
		endP = tagP;
		tagP = tagP->next;
	}
	if (endP!=pathList.backNode())
	{
		logDebug("%s\n",GetStringAddress(title));
		assert(endP==pathList.backNode());
	}

	endP = NULL;
	tagP = collector.frontNode();
	while(tagP)
	{
		if (isOut)
		{
			logDebug("%s>>%s\n",GetStringAddress(title),GetStringAddress(tagP->bufFirst));
		}
		if (tagP->lifeTag)
		{
			logDebug("%s%s\n",GetStringAddress(title),GetStringAddress(tagP->bufFirst));
			assert("0049错误发现"==NULL);
		}
		tagP->lifeTag = true;
		endP = tagP;
		tagP = tagP->next;
	}
	if (endP!=collector.backNode())
	{
		logDebug("%s\n",GetStringAddress(title));
		assert(endP==collector.backNode());
	}

	tagP = pathList.frontNode();
	while(tagP)
	{
		tagP->lifeTag = false;
		tagP = tagP->next;
	}
	tagP = collector.frontNode();
	while(tagP)
	{
		tagP->lifeTag = false;
		tagP = tagP->next;
	}
#endif
}

int DealClass::dealFilesInNewPath(
	std::string iterSrcPath,std::string iterDesPath
	)
{
	PathStruct* p = NULL;
	PathStruct* q = NULL;
	uChar errorNum = 0 ;
	PathStruct* currentPath = NULL ;

	srcNameSelector.clearNumString();
	desNameSelector.clearNumString();

	while(!pathList.isEmpty())
	{
		currentPath = pathList.frontNode();
#if Life_Tag
		this->test("0050");
#endif
		do
		{
			if (FILE_TYPE==currentPath->buf_type)
			{
				logFile("处理文件\"%s\"\n",GetStringAddress(currentPath->bufFirst));
//				break;
				if (paraData->modelType==CODE_MODE)
				{
					errorNum = dealFileObj.codeFile(
						iterSrcPath+currentPath->bufFirst,
						currentPath->bufFirst,
						iterDesPath,
						&desNameSelector
						);
				}
				else if (paraData->modelType==UNCODE_MODEL)
				{
					PathStruct desFolderPathStruct;
					desFolderPathStruct.bufFirst = iterDesPath ;
					errorNum = this->unCodeFile(
						iterSrcPath+currentPath->bufFirst,
						&desFolderPathStruct
						);
					if (errorNum==0)
					{
						iterDesPath = desFolderPathStruct.bufFirst ;
					}
				}
				else
				{
					logError("暂不支持\n");
				}

				if (errorNum)
				{
					logError("加解文件\"%s%s\"失败：%u\n",
						GetStringAddress(iterSrcPath),
						GetStringAddress(currentPath->bufFirst),
						errorNum);
				}
				if (isRenameFile)
				{
					if(false==dealFileObj.renameFile(iterSrcPath,
						currentPath->bufFirst,&srcNameSelector))
					{
						logError("重命名文件\"%s%s\"失败\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst));
					}
				}
				break;
			}

			if (FOLDER_TYPE==currentPath->buf_type)
			{
				logFile("处理文件夹\"%s\"\n",GetStringAddress(currentPath->bufFirst));
				//对于加密的情况将文件夹的相关信息存储起来
				errorNum = dealFolderObj.creatFolder(iterDesPath,&desNameSelector);
				if (errorNum==0)
				{
					p = collector.popFrontNode();
					p->buf_type = NEW_DES_PATH;
					p->bufFirst = iterDesPath + desNameSelector.numString + "\\";
					pathList.pushBackNode(p);

					if (paraData->modelType==CODE_MODE)
					{
						//在目标区域创建文件夹，在文件夹下创建相应的标志文件，将数据信息写入文件
						errorNum = dealFolderObj.codeFoder(p->bufFirst,currentPath->bufFirst);
						if (errorNum)
						{
							logError("对于源文件夹名\"%s\"在文件夹\"%s\"下进行标记失败：%u\n",
								GetStringAddress(currentPath->bufFirst),
								GetStringAddress(p->bufFirst),
								errorNum
								);
						}
					}
				}
				else
				{
					logError("目标文件夹\"%s\"下创建任意新文件夹失败,加密后的文件路径会有问题\n",
						GetStringAddress(iterDesPath));
				}

				if (isRenameFolder)
				{//将原文件夹重命名
					errorNum = dealFolderObj.renameFolder(
						iterSrcPath+currentPath->bufFirst,
						iterSrcPath,&srcNameSelector);
					if (0==errorNum)
					{
						currentPath->bufFirst = srcNameSelector.numString;
					}
					else
					{
						logError("重命名源文件夹\"%s%s\"失败：%u\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst),
							errorNum);
					}
				}

				p = collector.popFrontNode();
				p->buf_type = NEW_SRC_PATH;
				p->bufFirst = iterSrcPath + currentPath->bufFirst + "\\";
				pathList.pushBackNode(p);
				//存储文件夹的完整路径
				this->collectFilesInFolder(p->bufFirst);
				break;
			}
			if (NEW_SRC_PATH==currentPath->buf_type)
			{
				//int index = currentPath->bufFirst.find_first_of('\?');
				//iterSrcPath = currentPath->bufFirst.substr(0,index) ;
				//iterDesPath = currentPath->bufFirst.substr(index+1,currentPath->bufFirst.length());
				iterSrcPath = currentPath->bufFirst ;
				logFile("源文件夹进入\"%s\"\n",GetStringAddress(iterSrcPath));
				srcNameSelector.clearNumString();
				break;
			}
			if (NEW_DES_PATH==currentPath->buf_type)
			{
				iterDesPath = currentPath->bufFirst ;
				logFile("目标文件夹进入\"%s\"\n",GetStringAddress(iterDesPath));
				desNameSelector.clearNumString();
				break;
			}
		} while (0);
		pathList.popFrontNode();
		collector.pushBackNode(currentPath);
	}
	return 0 ;
}

//folderpath为 ..\\..\\的形式
void DealClass::collectFilesInFolder( std::string folderPath)
{
	PathStruct* p = NULL ;
	_finddata_t fileInfo;
	long handle = _findfirst(string(folderPath + "*").c_str(), &fileInfo);
	if (handle != -1L)
	{
		do{
			if (fileInfo.attrib &_A_SUBDIR)
			{
				//系统在进入一个子目录时，匹配到的头两个文件(夹)是"."(当前目录)，".."(上一层目录)。
				if( (strcmp(fileInfo.name,".") != 0 ) && (strcmp(fileInfo.name,"..") != 0))
				{
					p = collector.popFrontNode();
					p->buf_type = FOLDER_TYPE ;
					p->bufFirst = fileInfo.name ;
					pathList.pushBackNode(p);
					assert(p!=testPointer);
				}
			}
			else
			{
				p = collector.popFrontNode();
				p->buf_type = FILE_TYPE ;
				p->bufFirst = fileInfo.name ;
				pathList.pushBackNode(p);
			}
		}while(_findnext(handle, &fileInfo) == 0);
	}
	_findclose(handle);
}

int DealClass::dealFiles( PathPair* pathPair )
{
	string srcRoad = pathPair->srcPath;
	string iterSrcPath;
	string iterSrcName;

	bool isFolder = MyFileInfo::IsDirectory(srcRoad.data());
	if (!isFolder)
	{
		if (!MyFileInfo::isFileExits(srcRoad))
		{
			logError("文件\"%s\"不存在\n",GetStringAddress(srcRoad));
			return 0;
		}
	}

	size_t pos =  srcRoad.find_last_of('\\') + 1 ;
	iterSrcPath = srcRoad.substr(0,pos);
	iterSrcName = srcRoad.substr(pos,srcRoad.length());

	if (iterSrcName.empty()||iterSrcPath.empty())
	{
		logError("路径无文件名\n");
		return 0 ;
	}

	logFile("从处理\"%s\"开始，isFolder:%d\n",GetStringAddress(iterSrcPath),isFolder);


	PathStruct* p = collector.popFrontNode() ;
	if (isFolder)
	{
		p->buf_type = FOLDER_TYPE ;
	}
	else
	{
		p->buf_type = FILE_TYPE;
	}
	p->bufFirst = iterSrcName ;
	pathList.pushBackNode(p);

	this->test("0015");	

	if (isToNewPath)
	{
		if (pathPair->desPath.empty())
		{
			this->dealFilesInNewPath(iterSrcPath,iterSrcPath);
		}
		else
		{
			this->dealFilesInNewPath(iterSrcPath,pathPair->desPath+"\\");
		}
	}
	else
	{
		this->dealFilesInSamePath(iterSrcPath);
	}
	return 0 ;
}

int DealClass::dealFilesInSamePath(std::string iterSrcPath )
{
	uChar errorNum = 0 ;
	PathStruct* currentPath = NULL ;
	PathStruct* p = NULL ;

//	logError("%s\n",GetStringAddress(iterSrcPath));

	srcNameSelector.clearNumString();

	while(!pathList.isEmpty())
	{
		currentPath = pathList.frontNode();
		do
		{
			if (FILE_TYPE==currentPath->buf_type)
			{
				logFile("处理文件：\"%s\"\n",GetStringAddress(currentPath->bufFirst));
				if (isRenameFile)
				{
					if(
						dealFileObj.renameFile(iterSrcPath,
						currentPath->bufFirst,&srcNameSelector)
						)
					{
						currentPath->bufFirst = srcNameSelector.numString ;
					}
					else
					{
						logError("重命名文件：\"%s%s\"失败\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst));
					}
				}

				if (isDestroyFile)
				{
					errorNum = dealFileObj.destroyFile(iterSrcPath + currentPath->bufFirst);
				}
				else
				if (this->paraData->modelType==CHECK_MODEL)
				{
					errorNum = dealFileObj.checkFile(iterSrcPath+currentPath->bufFirst);
				}
				else
				{
					errorNum = 0 ;
				}

				if (errorNum)
				{
					logError("处理文件\"%s%s\"失败：%u\n",
						GetStringAddress(iterSrcPath),GetStringAddress(currentPath->bufFirst),
						errorNum);
				}
				break;
			}

			if (FOLDER_TYPE==currentPath->buf_type)
			{
				logFile("处理文件夹\"%s\"\n",GetStringAddress(currentPath->bufFirst));

				//对于加密的情况将文件夹的相关信息存储起来
				if (isRenameFolder)
				{//将原文件夹重命名
					errorNum = dealFolderObj.renameFolder(
						iterSrcPath+currentPath->bufFirst,
						iterSrcPath,&srcNameSelector);
					if (0==errorNum)
					{
						currentPath->bufFirst = srcNameSelector.numString;
					}
					else
					{
						logError("重命名源文件夹：\"%s%s\"失败：%d\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst),
							errorNum);
					}
				}

				p = collector.popFrontNode();
				p->buf_type = NEW_SRC_PATH;
				p->bufFirst = iterSrcPath + currentPath->bufFirst + "\\";
				pathList.pushBackNode(p);
				//存储文件夹的完整路径
				this->collectFilesInFolder(p->bufFirst);
				break;
			}

			if (NEW_SRC_PATH==currentPath->buf_type)
			{
				logFile("源进入：\"%s\"\n",GetStringAddress(iterSrcPath));
				iterSrcPath = currentPath->bufFirst ;
				srcNameSelector.clearNumString();
				break;
			}
		} while (0);
		pathList.popFrontNode();
		collector.pushBackNode(currentPath);
	}
	return 0 ;
}


uChar DealClass::unCodeFile(
	string srcFilePath,
	PathStruct* desFolderPath
	)
{
	uChar errorNum  = 0 ;
	do
	{
		SmartFiler smartFiler;
		errorNum = smartFiler.openFile(srcFilePath,Smart_Read_Flag) ;
		if (errorNum)
		{
			logError("打开文件\"%s\"失败:%d\n",GetStringAddress(srcFilePath),errorNum);
			break;
		}
		if ((smartFiler.getFileSectionOffset()==0)
			&&(smartFiler.getFileSectionCount()==0))
		{
			break;
		}
		if (false==smartFiler.getNexSection(Tailer_Max_Bytes_Size,true))
		{
			logError("映射文件\"%s\"失败\n",GetStringAddress(srcFilePath));
			errorNum = 1 + SmartFiler_ErrorNum_Max_OpenFile ;
			break;
		}
		dealInfoObj.dealByte2Info(smartFiler.getSectionAddress(),smartFiler.getSectionSize());
//		smartFiler.closeFile();
		if (0==tailerObj.getInfoSize())
		{
			logError("不可识别文件\"%s\"\n",GetStringAddress(srcFilePath));
			errorNum = 2 + SmartFiler_ErrorNum_Max_OpenFile ;
			break;
		}

		if (dealFolderObj.isFolderMark())
		{
			std::string oldName = desFolderPath->bufFirst.substr(0,desFolderPath->bufFirst.length()-1);
			string::size_type index = oldName.find_last_of('\\');
			if (index==string::npos)
			{
				logError("目标路径\"%s\"为不可重命名路径\n",
					GetStringAddress(desFolderPath->bufFirst));
				errorNum = 3 + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}
			std::string newName = oldName.substr(0,index+1)+
				std::string(tailerObj.getInfoAddress()+Folder_Header_Length);


			if (0==dealFolderObj.renameFolder(oldName,newName))
			{
				desFolderPath->bufFirst = newName + "\\" ;
				break;
			}
			NameSelector tmpSelector;
			newName = newName + "_" ;
			errorNum = dealFolderObj.renameFolder(oldName,newName,&tmpSelector);
			if (errorNum)
			{
				logError("目标路径\"%s\"重命名\"%s_*\"失败:%u\n",
					GetStringAddress(desFolderPath->bufFirst),
					GetStringAddress(newName),errorNum);
				errorNum = 4 + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}
			desFolderPath->bufFirst = newName + tmpSelector.numString + "\\";			
//			dealFolderObj.renameFolder();
			break;
		}
		if (dealFileObj.isFileMark())
		{
			string name = desFolderPath->bufFirst +
				std::string(tailerObj.getInfoAddress()+File_Header_Length);

			if (MyFileInfo::isFileExits(name))
			{
				NameSelector tmpSelector;
				string::size_type index = name.find_last_of('.');
				if (index==string::npos)
				{
					if (false==tmpSelector.getNextNumString(
						MyFileInfo::isFileExits,
						name)
						)
					{
						logError("无扩展名的文件取名失败\n");
						errorNum = 5 + SmartFiler_ErrorNum_Max_OpenFile ;
						break;
					}
					name = name + tmpSelector.numString ;
				}
				else
				{
					string ext = name.substr(index,name.length());
					name = name.substr(0,index) + "_";
					if (
						0==tmpSelector.getNextNumString(
							MyFileInfo::isFileExits,
							name,ext
						)
						)
					{
						logError("有扩展名的文件取名失败\n");
						errorNum = 6 + SmartFiler_ErrorNum_Max_OpenFile ;
						break;
					}
					name = name + tmpSelector.numString + ext ;
				}
			}
//			smartFiler.showMsg();
			errorNum = dealFileObj.unCodeFile(&smartFiler,name);
			if (errorNum)
			{
				errorNum += (7 + SmartFiler_ErrorNum_Max_OpenFile) ;
				break;
			}
			break;
		}
	} while (0);
	return errorNum;
}