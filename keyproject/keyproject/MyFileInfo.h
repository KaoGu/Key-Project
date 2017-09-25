#ifndef MyFileInfo_H
#define MyFileInfo_H

#include <Windows.h>
#include <string>
using namespace std;

class MyFileInfo
{
public:
	static bool IsDirectory(const char* filename);

	static bool isFolderExits(string folderPath);	
	static bool isFileExits(string filePath);

	static bool creatSimpleFolder(string folderPath);

	static int reName(const char *name, const char *newName);

	static bool quickCreateFileBigFile(std::string pathName,DWORD dwFileSizeHigh,DWORD dwFileSizeLow);

	static bool quickCreatePath(std::string path);

};



#endif
