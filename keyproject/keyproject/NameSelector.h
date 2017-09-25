#ifndef NameSelector_H
#define NameSelector_H

#include "GlobleType.h"
#include <string.h>
using namespace std;

class NameSelector
{
public:
	NameSelector();
	~NameSelector();
	void clearNumString();
	bool getNextNumString( bool (*check)(string),string pathName );
	bool getNextNumString( bool (*check)(string),string pathName ,string ext);
	string numString ;
private:
	char numChr[ MAX_NEW_NAME_LEN + 1 ];
};

#endif
