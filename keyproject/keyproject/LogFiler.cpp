#include "LogFiler.h"
#include <stdarg.h>

LogFiler logFiler;


/*
va_list、va_start、va_arg、va_end相关宏可以在stdarg.h中找到。

// _INTSIZEOF(n)宏：将sizeof(n)按sizeof(int)对齐。 
#define _INTSIZEOF(n)   ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )

// 取format参数之后的第一个变参地址，4字节对齐 
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )                    

// 对type类型数据，先取到其四字节对齐地址，再取其值 
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

// 将指针置为无效    
#define va_end(ap)    ( ap = (va_list)0 )        
*/

LogFiler::LogFiler(void)
{
}

LogFiler::~LogFiler(void)
{
}

void _cdecl LogFiler::Out(int level,char* format,... )
{

/*
	// 1: 首先在函数里定义一具va_list型的变量，这个变量是指向参数的指针
	va_list arg_ptr;

	// 2: 然后用va_start宏初始化变量刚定义的va_list变量，
	//    这个宏的第二个参数是第一个可变参数的前一个参数，也就是最后一个固定的参数。
	//    va_start返回的是一个可变参数地址
	va_start(arg_ptr, format);

	std::cout<<"str = "<<format<<std::endl;

//	printf(format);

	unsigned int i = 0 ;
	while(format[i]!='\0')
	{
		if (format[i]=='\\')
		{
			std::cout<<"遇到\\"<<std::endl;
		}
		i++;
	}

	// va_end宏结束可变参数的获取
	va_end(arg_ptr);
*/
}