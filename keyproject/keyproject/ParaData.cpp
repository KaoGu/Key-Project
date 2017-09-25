#include "ParaData.h"
#include "AnalyticParameter.h"
#include "MyFileInfo.h"
#include "LogFiler.h"
using namespace std;



void ParaData::findModel( AnalyticParameter &analyPara )
{
	do
	{
		int index = 0 ;

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

			index = analyPara.findArg("-fill_model");
			if (index>=0)
			{
				this->modelType = FILL_MODEL;
				srcFlag = All_False_Flag;
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

		index = analyPara.findArg("-check");
		if (index>=0)
		{
			this->srcFlag = Check_Flag + (this->srcFlag&To_New_Path_Flag);
		}

		logDebug("srcflag = %d\n",this->srcFlag);

		{//初始化pathList
			string desPath ;
			if ((this->srcFlag&(To_New_Path_Flag|Check_Flag))==To_New_Path_Flag)
			{
				logDebug("检测目标路径\n");
				index = analyPara.findArg("-despath") + 1 ;
				if ( (index>0)&&(index<analyPara.getArgCount()) )
				{//包含加密路径
					desPath = analyPara.getArg(index);					
					if (false==MyFileInfo::quickCreatePath(desPath))
					{
						logError("目标路径\"%s\"存在问题\n",
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
//				this->modelType = UNDEFINE_MODEL;
				break;
			}

			if (this->srcFlag&Check_Flag)
			{
				desPath = "?";
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
	logOut("\
		  \n\
		  LOGPATH\n\
		  \n\
		  CODE_MODE\n\
		  UNCODE_MODEL\n\
		  DESTROY_MODEL\n\
		  FILL_MODEL\n\
		  MKFILE path size\n\
		  \n\
		  SRCFLAG\n\
		  {\n\
			All_False_Flag\n\
			Destroy_File_Flag\n\
			Rename_File_Flag\n\
			Rename_Folder_Flag\n\
			To_New_Path_Flag\n\
			Check_Flag\n\
		  }\n\
		  \n\
		  CHECK\n\
		  \n\
		  uChar key[256];\n\
		  \n\
		  DESPATH\n\
		  SRCPATH\n\
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
