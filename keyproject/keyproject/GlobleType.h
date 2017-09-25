#ifndef GlobleType_H
#define GlobleType_H
#include <string>


#define Life_Tag 0


#define MAX_NEW_NAME_LEN 26

#define IS_TEST 1
typedef unsigned char uChar;


#define All_False_Flag 0
#define Destroy_File_Flag 1
#define Rename_File_Flag 2
#define Rename_Folder_Flag 4
#define To_New_Path_Flag 8
#define Check_Flag 16


typedef unsigned long uSmartSizeType;

typedef unsigned int Contain_Flag_TYPE;


typedef enum
{
	UNDEFINE_MODEL,
	CODE_MODE,
	UNCODE_MODEL,
	DESTROY_MODEL,
	FILL_MODEL,
	DEAL_OVER
}MODEL_TYPE;

typedef enum
{
	FOLDER_TYPE,
	FILE_TYPE,
	NEW_SRC_PATH,
	NEW_DES_PATH
}BUF_TYPE;

typedef struct pathstruct
{
#if Life_Tag
	bool lifeTag;
#endif
	BUF_TYPE buf_type;		
	std::string bufFirst;
//	std::string bufSecond;
	struct pathstruct *next;
}PathStruct;

typedef struct pathpair
{
	struct pathpair *next;
#if Life_Tag
	bool lifeTag;
#endif
	std::string srcPath;
	std::string desPath;
}PathPair;


#endif