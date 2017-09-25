#include "Tailer.h"
#include <time.h>
#include <assert.h>

Tailer::Tailer(uChar *keys)
	:keys(keys),
	bytes(new uChar[Tailer_Init_Captain_Size+Tailer_Init_Captain_Size]),
	byteSize(0),
	captainSize(Tailer_Init_Captain_Size+Tailer_Init_Captain_Size)
{
	this->initPre32Bytes(pre32Bytes);
	memcpy(bytes,pre32Bytes,32);
}

Tailer::~Tailer()
{
	if (bytes)
	{
		delete[] bytes;
		bytes = NULL;
	}
}


void Tailer::dealInfo2Byte()
{
	//此时信息放在36开始到0的位置
	srand(time(NULL));
	uTailerInt i = 0 , j = 0 ;

	assert(byteSize>2*infoSize+Tailer_Info_Start_Position);

	i = Tailer_Info_Start_Position + infoSize ;
	j = byteSize ;
	while(i>Tailer_Info_Start_Position)
	{
		--i;
		j-=2;
		bytes[j] = bytes[i]&0x0f;
		bytes[j+1] = ((uTailerInt)bytes[i]>>4)&0x0f;		
	}

	uTailerInt n = byteSize-36 ;
	uChar k = 16 ;//16-32
	uTailerInt infoCount = infoSize*2;
	
	i = 36 ;
	while(n)
	{
		if (rand()%n<infoCount)
		{
			--infoCount;
			assert(i<=j);
			assert((bytes[j]&0xf0)==0);
			bytes[i] = bytes[j]^keys[k];
			assert((bytes[i]&0xf0)==(keys[k]&0xf0));
			++j;
		}
		else
		{
			bytes[i] = rand()%16 + ( (((uTailerInt)keys[k])&0x00f0) + ((rand()%15+1)<<4) ) ;
			assert((bytes[i]&0xf0)!=(keys[k]&0xf0));
		}
		if (k>=31)
		{
			k = 16 ;
		}
		else
		{
			++k;
		}
		++i;
		--n;
	}
	assert(j==byteSize);

	bytes[35] = byteSize&0x000000ff;
	bytes[34] = (byteSize>>8)&0x000000ff;
	bytes[33] = (byteSize>>16)&0x000000ff;
	bytes[32] = 0;

}

void Tailer::dealByte2Info( uChar *fileBytes ,uTailerInt fileBytesSize)
{
	infoSize = 0 ;
	do
	{

		if (fileBytesSize<Tailer_Init_Captain_Size)
		{
			break;
		}

		uTailerInt i = 0 , j = 0 ;
		uChar k = 0 ;

		if (fileBytes[32]!=0)
		{
			break;
		}
		if (((uTailerInt)fileBytes[35])&Tailer_Init_Captain_Size_Low_Mark)
		{
			break;
		}

		for(i=0;(i<32)&&(pre32Bytes[i]==fileBytes[i]);++i){}
		if (i!=32)
		{
			break;
		}

		byteSize = ((uTailerInt)fileBytes[35])|
			(((uTailerInt)fileBytes[34])<<8)|
			(((uTailerInt)fileBytes[33])<<16);

		if ((byteSize>Tailer_Max_Bytes_Size)||
			(byteSize>fileBytesSize)
			)
		{
			break;
		}

		if (byteSize>captainSize)
		{
			this->resetByteCaptainSize(byteSize);
		}

		i = 36 ;
		j = 36 ;
		k = 16 ;


		while(j<byteSize)
		{
			if ((fileBytes[j]&0xf0)==(keys[k]&0xf0))
			{
				bytes[i] = ((fileBytes[j]^keys[k])&0x0f);
				++i;
			}
			if (k>=31)
			{
				k = 16;
			}
			else
			{
				++k;
			}
			++j;
		}

		if ((i%2)||
			(
			byteSize/Tailer_Init_Captain_Size!=
			((i+28)/Tailer_Init_Captain_Size+1)
			)
			)
		{
			break;
		}
		infoSize = i ;
		for ( i = 36 , j = 36 ; j<infoSize ; ++i , j += 2 )
		{
			bytes[i] = bytes[j]+(bytes[j+1]<<4);
		}
		infoSize = i - 36 ;
	} while (0);
	bytes[Tailer_Info_Start_Position+infoSize] = '\0' ;
}

void Tailer::initPre32Bytes(uChar *arr)
{
	char mark[33] = {"baidu百度一下，你就知道"};
	for(uChar i = 0 ; i<32 ; i++)
	{
		arr[i] = mark[i]^i;
	}
}

void Tailer::resetByteCaptainSize( uTailerInt newCaptainSize )
{
	delete[] bytes;
	captainSize = newCaptainSize;
	bytes = new uChar[captainSize];
	memcpy(bytes,pre32Bytes,32);
}

void Tailer::setInfoSize( uTailerInt len )
{
	if (len>Tailer_Max_Info_Size)
	{
		infoSize = Tailer_Max_Info_Size;
	}
	else
	{
		infoSize = len;
	}
	byteSize = ((2*infoSize+64)/Tailer_Init_Captain_Size + 1)*Tailer_Init_Captain_Size;
	if (byteSize>captainSize)
	{
		this->resetByteCaptainSize(byteSize);
	}
}

