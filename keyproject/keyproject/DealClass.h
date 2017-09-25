#ifndef DealClass_H
#define DealClass_H
#include <string>
using namespace std;

#include "GlobleType.h"
#include "ParaData.h"
#include "MyList.h"
#include "NameSelector.h"
#include "Tailer.h"
#include "DealInfoClass.h"
#include "DealFileClass.h"
#include "DealFolderClass.h"

#define Deal_ErrorNum_Max_UnCodeFile ( 7 + SmartFiler_ErrorNum_Max_OpenFile + DealFile_ErrorNum_Max_UnCodeFile )



class DealClass
{
public:
	DealClass(ParaData *paraData);
	void collectFilesInFolder(std::string folderPath);
	int dealFiles(PathPair* pathPair);
	int dealFilesInSamePath(std::string iterSrcPath);
	int dealFilesInNewPath(std::string iterSrcPath,std::string iterDesPath);
	

	uChar unCodeFile(	string srcFilePath,
		PathStruct* desFolderPath
		);
	void test(std::string title,bool isOut = false);
	PathStruct* testPointer;
	uChar testFlag;
	
private:
	ParaData *paraData;
	TypeClass<PathStruct>::Collector collector;
	TypeClass<PathStruct>::List pathList;

	NameSelector srcNameSelector;
	NameSelector desNameSelector;

	Tailer tailerObj;
	DealInfoClass dealInfoObj;
	DealFileClass dealFileObj;
	DealFolderClass dealFolderObj;

	bool isDestroyFile;
	bool isRenameFile;
	bool isRenameFolder;
	bool isToNewPath;
};


#endif

