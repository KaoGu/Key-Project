#include "NameSelector.h"


NameSelector::NameSelector(void)
{
	numChr[0] = 'a' - 1 ;
	numChr[1] = '\0';
}


void NameSelector::clearNumString()
{
	numChr[0] = 'a' - 1 ;
	numChr[1] = '\0';
}

bool NameSelector::getNextNumString( bool (*check)(string),string pathName )
{
	int i = 0 ;
	do
	{
		i = 0 ;
		while(numChr[i]>='z')//遇到'\0'也会结束
		{
			numChr[i] = 'a' ;
			++i;
		}
		if (numChr[i]=='\0')
		{
			if (i>=MAX_NEW_NAME_LEN)
			{
				break;
			}
			numChr[i]='a';
			numChr[i+1] = '\0';
		}
		else
		{
			numChr[i]++;
		}
		numString = string(numChr);
		if (!(*check)(pathName+numString))
		{
			break;
		}
	} while (1);
	return i<MAX_NEW_NAME_LEN;
}

bool NameSelector::getNextNumString( bool (*check)(string),string pathName ,string ext )
{
	int i = 0 ;
	do
	{
		i = 0 ;
		while(numChr[i]>='z')//遇到'\0'也会结束
		{
			numChr[i] = 'a' ;
			++i;
		}
		if (numChr[i]=='\0')
		{
			if (i>=MAX_NEW_NAME_LEN)
			{
				break;
			}
			numChr[i]='a';
			numChr[i+1] = '\0';
		}
		else
		{
			numChr[i]++;
		}
		numString = string(numChr);
		if (!(*check)(pathName+numString + ext))
		{
			break;
		}
	} while (1);
	return i<MAX_NEW_NAME_LEN;
}


NameSelector::~NameSelector(void)
{
}
