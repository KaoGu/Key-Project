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
	isCheck(paraData->srcFlag&Check_Flag)
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
			logDebug("%spathlist�з���testpointer\n",GetStringAddress(title));
		}
		if (tagP->lifeTag)
		{
			logDebug("%s%s\n",GetStringAddress(title),GetStringAddress(tagP->bufFirst));
			assert("0048������"==NULL);
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
			assert("0049������"==NULL);
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
	logFile("��·���´���\n");

	bool isFindFolderMark = true;

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
				logFile("�����ļ�\"%s\"\n",GetStringAddress(currentPath->bufFirst));
//				break;
				if (paraData->modelType==CODE_MODE)
				{
					if (isCheck)
					{
						errorNum = dealFileObj.checkFile(iterSrcPath+currentPath->bufFirst);
					}
					else
					{
						errorNum = dealFileObj.codeFile(
							iterSrcPath+currentPath->bufFirst,
							currentPath->bufFirst,
							iterDesPath,
							&desNameSelector
							);
					}
				}
				else if (paraData->modelType==UNCODE_MODEL)
				{
					PathStruct desFolderPathStruct;
					desFolderPathStruct.bufFirst = iterDesPath ;
					errorNum = this->unCodeFile(
						iterSrcPath+currentPath->bufFirst,
						&desFolderPathStruct
						);
					if (errorNum<8)
					{						
						if (errorNum==1)
						{
							logFile("������ļ��У���\"%s\"����===>\"%s\"\n",
								GetStringAddress(iterDesPath),
								GetStringAddress(desFolderPathStruct.bufFirst));
							iterDesPath = desFolderPathStruct.bufFirst ;
							if (isFindFolderMark)
							{
								logWarn("�ļ����·��ֶ����־�ļ�\n");
							}
							else
							{
								isFindFolderMark = true ;
							}
						}
						else
							if (errorNum==2)
							{
								logFile("����������ļ���Ϊ\"%s\"\n",
									GetStringAddress(desFolderPathStruct.bufFirst)
									);
							}
							else
							{
								logError("������һ��������������\n");
							}
						errorNum = 0 ;
					}
				}
				else
				{
					errorNum = 0 ;
					logError("�ݲ�֧��\n");
				}

				if (errorNum)
				{
					logError("�ӽ��ļ�\"%s%s\"ʧ�ܣ�%u\n",
						GetStringAddress(iterSrcPath),
						GetStringAddress(currentPath->bufFirst),
						errorNum);
				}
				if (isRenameFile)
				{
					if(false==dealFileObj.renameFile(iterSrcPath,
						currentPath->bufFirst,&srcNameSelector))
					{
						logError("�������ļ�\"%s%s\"ʧ��\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst));
					}
				}
				break;
			}

			if (FOLDER_TYPE==currentPath->buf_type)
			{
				logFile("�����ļ���\"%s\"\n",GetStringAddress(currentPath->bufFirst));
				//���ڼ��ܵ�������ļ��е������Ϣ�洢����
				if (false==isCheck)
				{
					errorNum = dealFolderObj.creatFolder(iterDesPath,&desNameSelector);
					if (errorNum==0)
					{
						p = collector.popFrontNode();
						p->buf_type = NEW_DES_PATH;
						p->bufFirst = iterDesPath + desNameSelector.numString + "\\";
						pathList.pushBackNode(p);

						if (paraData->modelType==CODE_MODE)
						{
							//��Ŀ�����򴴽��ļ��У����ļ����´�����Ӧ�ı�־�ļ�����������Ϣд���ļ�
							errorNum = dealFolderObj.codeFoder(p->bufFirst,currentPath->bufFirst);
							if (errorNum)
							{
								logError("����Դ�ļ�����\"%s\"���ļ���\"%s\"�½��б��ʧ�ܣ�%u\n",
									GetStringAddress(currentPath->bufFirst),
									GetStringAddress(p->bufFirst),
									errorNum
									);
							}
						}
					}
					else
					{
						logError("Ŀ���ļ���\"%s\"�´����������ļ���ʧ��,���ܺ���ļ�·����������\n",
							GetStringAddress(iterDesPath));
					}
				}
				else
				{
					if (paraData->modelType==UNCODE_MODEL)
					{
						p = collector.popFrontNode();
						p->buf_type = NEW_DES_PATH;
						p->bufFirst = iterDesPath + "?\\";
						pathList.pushBackNode(p);
					}
				}

				if (isRenameFolder)
				{//��ԭ�ļ���������
					errorNum = dealFolderObj.renameFolder(
						iterSrcPath+currentPath->bufFirst,
						iterSrcPath,&srcNameSelector);
					if (0==errorNum)
					{
						currentPath->bufFirst = srcNameSelector.numString;
					}
					else
					{
						logError("������Դ�ļ���\"%s%s\"ʧ�ܣ�%u\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst),
							errorNum);
					}
				}

				p = collector.popFrontNode();
				p->buf_type = NEW_SRC_PATH;
				p->bufFirst = iterSrcPath + currentPath->bufFirst + "\\";
				pathList.pushBackNode(p);
				//�洢�ļ��е�����·��
				this->collectFilesInFolder(p->bufFirst);
				break;
			}
			if (NEW_SRC_PATH==currentPath->buf_type)
			{
				//int index = currentPath->bufFirst.find_first_of('\?');
				//iterSrcPath = currentPath->bufFirst.substr(0,index) ;
				//iterDesPath = currentPath->bufFirst.substr(index+1,currentPath->bufFirst.length());
				if (UNCODE_MODEL==paraData->modelType)
				{
					if (isFindFolderMark==false)
					{
						logWarn("��δ��Դ�ļ���\"%s\"���ҵ���־�ļ�\n",
							GetStringAddress(iterSrcPath));
					}
					else
					{
						isFindFolderMark = false;
					}
				}
				iterSrcPath = currentPath->bufFirst ;
				logFile("Դ�ļ��н���\"%s\"\n",GetStringAddress(iterSrcPath));
				srcNameSelector.clearNumString();
				break;
			}
			if (NEW_DES_PATH==currentPath->buf_type)
			{
				iterDesPath = currentPath->bufFirst ;
				logFile("Ŀ���ļ��н���\"%s\"\n",GetStringAddress(iterDesPath));
				desNameSelector.clearNumString();
				break;
			}
		} while (0);
		pathList.popFrontNode();
		collector.pushBackNode(currentPath);
	}
	return 0 ;
}

//folderpathΪ ..\\..\\����ʽ
//�ռ�ʱ���ռ��ļ����ռ��ļ���
void DealClass::collectFilesInFolder( std::string folderPath)
{
	TypeClass<PathStruct>::List folderList;

	PathStruct* p = NULL ;
	_finddata_t fileInfo;
	long handle = _findfirst(string(folderPath + "*").c_str(), &fileInfo);
	if (handle != -1L)
	{
		do{
			if (fileInfo.attrib &_A_SUBDIR)
			{
				//ϵͳ�ڽ���һ����Ŀ¼ʱ��ƥ�䵽��ͷ�����ļ�(��)��"."(��ǰĿ¼)��".."(��һ��Ŀ¼)��
				if( (strcmp(fileInfo.name,".") != 0 ) && (strcmp(fileInfo.name,"..") != 0))
				{
					p = collector.popFrontNode();
					p->buf_type = FOLDER_TYPE ;
					p->bufFirst = fileInfo.name ;
					folderList.pushBackNode(p);
//					pathList.pushBackNode(p);
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
	pathList.pushBackList(folderList);
}

int DealClass::dealFiles( PathPair* pathPair )
{
	uChar errorNum = 0 ;	
	do 
	{
		string srcRoad = pathPair->srcPath;

		bool isFolder = MyFileInfo::IsDirectory(srcRoad.data());
		if (!isFolder)
		{
			if (!MyFileInfo::isFileExits(srcRoad))
			{
				logError("�ļ�\"%s\"������\n",GetStringAddress(srcRoad));
				errorNum = 1 ;
				break;
			}
		}

		if (paraData->modelType==FILL_MODEL)
		{
            logFile("�ļ�-���\n");
			if (isFolder==false)
			{
				errorNum = 2 ;
				logError("·������һ���ļ���\n");
				break;
			}
			this->fillFolder(srcRoad+"\\");
			break;
		}

		size_t pos =  srcRoad.find_last_of('\\') + 1 ;
		string iterSrcPath = srcRoad.substr(0,pos);
		string iterSrcName = srcRoad.substr(pos,srcRoad.length());

		if (iterSrcName.empty()||iterSrcPath.empty())
		{
			logError("·�����ļ���\n");
			errorNum = 3 ;
			break;
		}

		logFile("�Ӵ���\"%s\"��ʼ��isFolder:%d\n",GetStringAddress(iterSrcPath),isFolder);

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


		if ((paraData->modelType==CODE_MODE)
			||(paraData->modelType==UNCODE_MODEL))
		{
            logFile("�ļ�-�ӽ���\n");
			if (pathPair->desPath.empty())
			{
				this->dealFilesInNewPath(iterSrcPath,iterSrcPath);
			}
			else
			{
				this->dealFilesInNewPath(iterSrcPath,pathPair->desPath+"\\");
			}
			break;
		}

		if (paraData->modelType==DESTROY_MODEL)
		{
            logFile("�ļ�-�ݻ�\n");
			this->dealFilesInSamePath(iterSrcPath);
			break;
		}
		errorNum = 4 ;
        logError("�ļ�-δʶ������\n");        
	} while (0);
	return errorNum ;
}

int DealClass::dealFilesInSamePath(std::string iterSrcPath )
{
	uChar errorNum = 0 ;
	PathStruct* currentPath = NULL ;
	PathStruct* p = NULL ;

	logFile("ԭ·���´���\n");

//	logError("%s\n",GetStringAddress(iterSrcPath));

	srcNameSelector.clearNumString();

	while(!pathList.isEmpty())
	{
		currentPath = pathList.frontNode();
		do
		{
			if (FILE_TYPE==currentPath->buf_type)
			{
				logFile("�����ļ���\"%s\"\n",GetStringAddress(currentPath->bufFirst));
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
						logError("�������ļ���\"%s%s\"ʧ��\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst));
					}
				}

				if (this->isCheck)
				{
					errorNum = dealFileObj.checkFile(iterSrcPath+currentPath->bufFirst);
				}
				else
				if (isDestroyFile)
				{
					errorNum = dealFileObj.destroyFile(iterSrcPath + currentPath->bufFirst);
				}
				else
				{
					errorNum = 0 ;
				}

				if (errorNum)
				{
					logError("�����ļ�\"%s%s\"ʧ�ܣ�%u\n",
						GetStringAddress(iterSrcPath),GetStringAddress(currentPath->bufFirst),
						errorNum);
				}
				break;
			}

			if (FOLDER_TYPE==currentPath->buf_type)
			{
				logFile("�����ļ���\"%s\"\n",GetStringAddress(currentPath->bufFirst));

				//���ڼ��ܵ�������ļ��е������Ϣ�洢����
				if (isRenameFolder)
				{//��ԭ�ļ���������
					errorNum = dealFolderObj.renameFolder(
						iterSrcPath+currentPath->bufFirst,
						iterSrcPath,&srcNameSelector);
					if (0==errorNum)
					{
						currentPath->bufFirst = srcNameSelector.numString;
					}
					else
					{
						logError("������Դ�ļ��У�\"%s%s\"ʧ�ܣ�%d\n",
							GetStringAddress(iterSrcPath),
							GetStringAddress(currentPath->bufFirst),
							errorNum);
					}
				}

				p = collector.popFrontNode();
				p->buf_type = NEW_SRC_PATH;
				p->bufFirst = iterSrcPath + currentPath->bufFirst + "\\";
				pathList.pushBackNode(p);
				//�洢�ļ��е�����·��
				this->collectFilesInFolder(p->bufFirst);
				break;
			}

			if (NEW_SRC_PATH==currentPath->buf_type)
			{
				logFile("Դ���룺\"%s\"\n",GetStringAddress(iterSrcPath));
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


//1��ʾ�ļ���2��ʾ�ļ���������ʾ�쳣,8��������Ԥ����ʾ�������
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
			errorNum += 8;
			logError("���ļ�\"%s\"ʧ��:%d\n",GetStringAddress(srcFilePath),errorNum);
			break;
		}
		if ((smartFiler.getFileSectionOffset()==0)
			&&(smartFiler.getFileSectionCount()==0))
		{
			logError("�ļ���\"%s\"��СΪ0\n",
				GetStringAddress(srcFilePath));
			errorNum = 9 + SmartFiler_ErrorNum_Max_OpenFile ;
			break;
		}
		if (false==smartFiler.getNexSection(Tailer_Max_Bytes_Size,true))
		{
			logError("ӳ���ļ�\"%s\"ʧ��\n",GetStringAddress(srcFilePath));
			errorNum = 10 + SmartFiler_ErrorNum_Max_OpenFile ;
			break;
		}
		dealInfoObj.dealByte2Info(smartFiler.getSectionAddress(),smartFiler.getSectionSize());
//		smartFiler.closeFile();
		if (0==tailerObj.getInfoSize())
		{
			logError("δ�����ļ���汾���Ի����벻��\"%s\"\n",GetStringAddress(srcFilePath));
			errorNum = 11 + SmartFiler_ErrorNum_Max_OpenFile ;
			break;
		}

		if (dealFolderObj.isFolderMark())
		{
			logFile("��������ļ�����\"%s\"\n",
				(tailerObj.getInfoAddress()+Folder_Header_Length));

			std::string oldName = desFolderPath->bufFirst.substr(0,desFolderPath->bufFirst.length()-1);
			string::size_type index = oldName.find_last_of('\\');
			if (index==string::npos)
			{
				logError("Ŀ��·��\"%s\"Ϊ����������·��\n",
					GetStringAddress(desFolderPath->bufFirst));
				errorNum = 12 + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}
			std::string newName = oldName.substr(0,index+1)+
				std::string(tailerObj.getInfoAddress()+Folder_Header_Length);

			if (isCheck)
			{
				desFolderPath->bufFirst = newName + "\\";
				errorNum = 1 ;
				break;
			}

			if (0==dealFolderObj.renameFolder(oldName,newName))
			{
				desFolderPath->bufFirst = newName + "\\" ;
				errorNum = 1 ;
				break;
			}
			NameSelector tmpSelector;
			newName = newName + "_" ;
			errorNum = dealFolderObj.renameFolder(oldName,newName,&tmpSelector);
			if (errorNum)
			{
				logError("Ŀ��·��\"%s\"������\"%s_*\"ʧ��:%u\n",
					GetStringAddress(desFolderPath->bufFirst),
					GetStringAddress(newName),errorNum);
				errorNum = errorNum + 13 + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}

			desFolderPath->bufFirst = newName + tmpSelector.numString + "\\";			
			errorNum = 1 ;

//			dealFolderObj.renameFolder();
			break;
		}
		if (dealFileObj.isFileMark())
		{
			logFile("��������ļ���\"%s\"\n",
				(tailerObj.getInfoAddress()+File_Header_Length));

			if (isCheck)
			{
				desFolderPath->bufFirst = std::string(tailerObj.getInfoAddress()+File_Header_Length);
				errorNum = 2 ;
				break;
			}

			string name = desFolderPath->bufFirst +
				std::string(tailerObj.getInfoAddress()+File_Header_Length);

			if (MyFileInfo::isFileExits(name))
			{
				NameSelector tmpSelector;
				string::size_type index = name.find_last_of('.');
				if (index==string::npos)
				{
					name = name + "_";
					if (false==tmpSelector.getNextNumString(
						MyFileInfo::isFileExits,
						name)
						)
					{
						logError("����չ�����ļ�ȡ��ʧ��\n");
						errorNum = 14 + DealFolder_ErrorNum_Max_RenameFolder3Arg + SmartFiler_ErrorNum_Max_OpenFile ;
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
						logError("����չ�����ļ�ȡ��ʧ��\n");
						errorNum = 15 + DealFolder_ErrorNum_Max_RenameFolder3Arg + SmartFiler_ErrorNum_Max_OpenFile ;
						break;
					}
					name = name + tmpSelector.numString + ext ;
				}
			}
//			smartFiler.showMsg();
			errorNum = dealFileObj.unCodeFile(&smartFiler,name);
			if (errorNum)
			{
				errorNum = 15 + errorNum + DealFolder_ErrorNum_Max_RenameFolder3Arg + SmartFiler_ErrorNum_Max_OpenFile ;
				break;
			}
			desFolderPath->bufFirst = name ;
			errorNum = 2 ;
			break;
		}
		logError("�����ļ�ͷ���޸�Ϊ����ʶ��\n");
		errorNum  = 8 ;
	} while (0);
	return errorNum;
}

uChar DealClass::fillFolder( string folderPath )
{
	uChar errorNum = 0 ;
	logDebug("�����ļ���\"%s\"\n",GetStringAddress(folderPath));
	uSmartSizeType secCount = 1<<31 ;
	uSmartSizeType secOffset = 0 ;

	desNameSelector.clearNumString();

	while(1)
	{
		if (errorNum==0)
		{
			if (false==desNameSelector.getNextNumString(
				MyFileInfo::isFileExits,folderPath))
			{
				logError("�޷�ȡ������ɴ���\n");
				errorNum = 1 ;
				break;
			}
		}

//			logDebug("%s\n",GetStringAddress(desNameSelector.numString));

		errorNum = SmartFiler::makeFile(folderPath+desNameSelector.numString,
			secCount,secOffset,SmartFiler_ErrorNum_MakeFile_ErrorSize);
		if (errorNum)
		{
			if(errorNum!=SmartFiler_ErrorNum_Max_MakeFile)
			{
				errorNum = 0 ;
				break;
			}
		}

		if (secCount)
		{
			secCount>>=1;
			if (secCount==0)
			{
				secOffset = 1<<(Smart_Section_Cell_Low-1) ;
			}
		}
		else
			if (secOffset)
			{
				secOffset>>=1;
				if (secOffset==0)
				{
					errorNum = 0 ;
					break;
				}
				continue;
			}
			else
			{
				errorNum = 3 ;
				break;
			}
	}

	return errorNum ;
}
