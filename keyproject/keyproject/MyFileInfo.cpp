#include "MyFileInfo.h"
#include "LogFiler.h"
#include <cstdlib>
#include <Windows.h>
#include <io.h>
#include <list>
#include <direct.h>

bool MyFileInfo::IsDirectory( const char* filename )
{
	DWORD dwAttr = ::GetFileAttributes(filename);  //得到文件属性	
	if (dwAttr == 0xFFFFFFFF)    // 文件或目录不存在
		return false;
	else if (dwAttr&FILE_ATTRIBUTE_DIRECTORY)  // 如果是目录
		return true; 
	else
		return false;
}

bool MyFileInfo::isFolderExits( string folderPath )
{
	if(_access(folderPath.data(),0)!=-1)
	{
		return true;
	}
	else
	{
		return false;
	}

	/*//此只检测文件夹，但文件和文件夹也不能同名
	bool flag = false;
	WIN32_FIND_DATA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(folderPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
	flag = true;
	}
	FindClose(hFind);
	return flag;
	*/
}


int MyFileInfo::reName( const char *name, const char *newName )
{
	return rename(name,newName);
}

bool MyFileInfo::isFileExits( string filePath )
{
		if(_access(filePath.data(),0)!=-1)
		{
			return true;
		}
		else
		{
			return false;
		}

/*	//此检测对刚创建的文件无效
		bool flag = false;
		FILE* fh = fopen(filePath.data(),"r");
		if(fh)
		{
			flag = true;
			fclose(fh);
		}
		return flag;
		*/
}

bool MyFileInfo::quickCreateFileBigFile( std::string pathName,DWORD dwFileSizeHigh,DWORD dwFileSizeLow )
{

	bool flag = FALSE ;
	FILE* fp = NULL ;
	fopen_s(&fp,pathName.c_str(),"w");
	if (fp==NULL)
	{
		logError("创建文件\"%s\"失败\n",GetStringAddress(pathName));
		return false;
	}
	fclose(fp);

	if ((dwFileSizeLow==0)&&(dwFileSizeHigh==0))
	{
		return true ;
	}
	HANDLE hFile = CreateFile(pathName.data(),GENERIC_WRITE|GENERIC_READ,0,NULL
		,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		logError("创建大文件\"%s\"失败\n",GetStringAddress(pathName));
		return FALSE;
	}
	HANDLE hFileMap = CreateFileMapping(hFile,NULL,PAGE_READWRITE,
		dwFileSizeHigh,dwFileSizeLow,NULL);
	if(hFileMap == NULL)
	{
		CloseHandle(hFile);
		DWORD errorNum = GetLastError();
		logError("错误码:%l\n",errorNum);
		return FALSE;
	}
	CloseHandle(hFileMap);
	CloseHandle(hFile);
	flag = true ;
	return flag;
}


bool MyFileInfo::quickCreatePath( std::string path )
{
	int index = 0 ;
	std::list<std::string> tmpList;

	while(!path.empty())
	{
		logDebug("检测");
		if (MyFileInfo::isFolderExits(path))
		{
			if (MyFileInfo::IsDirectory(path.c_str()))
			{
				break;
			}
			else
			{
				return false;
			}
		}
		else
		{
			tmpList.push_front(path);
			index = path.find_last_of('\\');			
			if (index==string::npos)
			{
				return false;
			}
			else
			{
				path = path.substr(0,index);
			}
		}
	}

	while(!tmpList.empty())
	{
		path = tmpList.front();
		logDebug("创建%s\n",GetStringAddress(path));
		if (!MyFileInfo::creatSimpleFolder(path))
		{
			return false;			
		}
		tmpList.pop_front();
	}
	return true ;	
}

bool MyFileInfo::creatSimpleFolder( string folderPath )
{
	return 0==_mkdir(folderPath.c_str());
}
