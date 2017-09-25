#include "ParaData.h"
#include "AnalyticParameter.h"
#include "MyFileInfo.h"
#include "LogFiler.h"
using namespace std;



ParaData::ParaData( AnalyticParameter &analyPara ) :
	modelType(UNDEFINE_MODEL),
	showType(NO_SHOW_NAME),
	srcFlag(All_False_Flag)
{
	do 
	{
		int index = 0 ;

		if (analyPara.findArg("-help")>=0)
		{
			this->showHelp();
			return;
		}

		do
		{
			//定义各模式的默认属性
			index = analyPara.findArg("-destroy_model");
			if (index>=0)
			{
				this->modelType = DESTROY_MODEL;
				srcFlag = Destroy_File_Flag|Rename_File_Flag|Rename_Folder_Flag;
				break;
			}

			index = analyPara.findArg("-check_model");
			if (index>=0)
			{
				this->modelType = CHECK_MODEL;
				break;
			}

			index = analyPara.findArg("-code_model");
			if (index>=0)
			{
				this->modelType = CODE_MODE;
				srcFlag = To_New_Path_Flag;
				break;																			
			}

			index = analyPara.findArg("-uncode_model");
			if (index>=0)
			{
				this->modelType = UNCODE_MODEL;
				srcFlag = To_New_Path_Flag;
				break;
			}
		} while (0);

		if (this->modelType==UNDEFINE_MODEL)
		{
			break;
		}

		index = analyPara.getArgValue("-srcflag") ;
		if (index>=0)
		{
			this->srcFlag = index ;
		}

		index = analyPara.getArgValue("-show_type") ;
		if (index>=0)
		{
			this->showType = (SHOW_TYPE)index ;
		}


		{//初始化pathList

			string desPath ;
			if (this->srcFlag&To_New_Path_Flag)
			{
				index = analyPara.findArg("-despath") + 1 ;
				if ( index>0 )
				{//包含加密路径
					desPath = analyPara.getArg(index);					
					if (false==MyFileInfo::quickCreatePath(desPath))
					{
						logError("目标路径\"%s\"存在问题",
							GetStringAddress(desPath));
						break;
					}
				}
			}

			//各种指定参数
			index = analyPara.findArg("-srcpath") + 1 ;
			int jndex = analyPara.getArgCount();
			if ( (index<=0)||(index>=jndex) )
			{
				this->modelType = UNDEFINE_MODEL;
				break;
			}

			while(index<jndex)
			{
				PathPair *p = new PathPair ;
#if Life_Tag
				p->lifeTag = true;
#endif
				pathList.pushBackNode(p) ;
				p->srcPath = analyPara.getArg(index) ;
				p->desPath = desPath ;
				index++ ;
			}
		}

		if (this->modelType==DESTROY_MODEL)
		{
			break;
		}

		uChar tmpKey[256] = {"\
1234567..8901234567890as\
kdfjiql...jfa_ksdd\
djfkjas_dif;akqoig\
x';jqw55s7f1a8saasdfk\
jakjsd.fk;..jksjdf;_ajiwqnjznc'\
qwpoekk	qwej...kkmxoaajskda\
o5*59018%@$&*9+ioqjwefi\
jsdkfj__;..jiwq;..iyiewjriuq\
iwjeqiwejqoieurqpweijaiopsdf;qo\
aksdjfiq;ojfia;siodufj;qfjiaj;fkas\
"};
		index = analyPara.findArg("-key")+1;
		if ((index>0)&&(index<analyPara.getArgCount()))
		{
			string keyStr = analyPara.getArg(index);
			unsigned int i = keyStr.length();
			if ((i>2)&&(keyStr[0]=='0')&&((keyStr[1]=='x')||(keyStr[1]=='X')))
			{
				uChar j = 0 ;
				while(i>2)
				{
					--i;
					tmpKey[j] = (tmpKey[j]<<4)|keyStr[i];
					j++;
				}
			}
			else
			{
				uChar j = 0 ;
				while(i)
				{
					--i;
					tmpKey[j] = keyStr[i] ;
					j += 3 ;
				}
				this->initByteKey(tmpKey);
			}
		}
		memcpy(this->key,tmpKey,sizeof(this->key)/sizeof(this->key[0]));

	} while (0);
}

void ParaData::showHelp()
{
	logDebug("\
		  \n\
		  CODE_MODE\n\
		  UNCODE_MODEL\n\
		  CHECK_MODEL\n\
		  DESTROY_MODEL\n\
		  FILE\n\
		  \n\
		  typedef enum\n\
		  {\n\
		  NO_SHOW_NAME = 0,\n\
		  SHOW_NAME,\n\
		  ONLY_SHOW_NAME	\n\
		  }SHOW_TYPE;\n\
		  \n\
		  uChar key[256];\n\
		  \n\
		  ");
}

void ParaData::initByteKey(uChar key[256])
{
	uChar i = 0 ;
	uChar j  = 251;
	while(i<j)
	{
		key[i] = key[i] + key[j];
		++i;
		--j;
	}
	j -= 17 ;
	while(i>j)
	{
		key[i] = key[i] + key[j];
		++i;
		--j;
	}
	j += 59 ;
	while(i<j)
	{
		key[i] = key[i] + key[j];
		++i;
		--j;
	}
	j -= 11 ;
	while(i>j)
	{
		key[i] = key[i] + key[j];
		++i;
		--j;
	}
}
