#include "DealInfoClass.h"



DealInfoClass::DealInfoClass( uChar* keys,Tailer* tailer )
	:keys(keys),tailerObj(tailer)
{
}


DealInfoClass::~DealInfoClass()
{
}

void DealInfoClass::dealByte2Info( uChar *fileBytes,uTailerInt fileBytesSize )
{
	tailerObj->dealByte2Info(fileBytes,fileBytesSize);

	uChar k = 0 ;
	uTailerInt i = tailerObj->getInfoSize();
	char* s = tailerObj->getInfoAddress() ;
	unsigned int v = 0 ;
	while(i)
	{
		--i;
		v = s[i];
		v = (v&0x0f) + ((v^keys[(v&0x0f)+8])&0xf0);
		s[i] = v^(keys[k]&0x1E) ;
		++k;
	}
}

void DealInfoClass::dealInfo2Byte()
{
	uChar k = 0 ;
	char* s = tailerObj->getInfoAddress();
	uTailerInt i = tailerObj->getInfoSize();
	unsigned int v = 0 ;
	while(i)
	{
		--i;
		v = s[i] ;
		v ^= (keys[k]&0x1E) ;
		s[i] = (v&0x0f) + ((v^keys[(v&0x0f)+8])&0xf0);
		++k;
	}
	tailerObj->dealInfo2Byte();

}
