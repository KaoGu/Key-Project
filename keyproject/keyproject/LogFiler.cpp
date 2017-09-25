#include "LogFiler.h"
#include <stdarg.h>

LogFiler logFiler;


/*
va_list��va_start��va_arg��va_end��غ������stdarg.h���ҵ���

// _INTSIZEOF(n)�꣺��sizeof(n)��sizeof(int)���롣 
#define _INTSIZEOF(n)   ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )

// ȡformat����֮��ĵ�һ����ε�ַ��4�ֽڶ��� 
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )                    

// ��type�������ݣ���ȡ�������ֽڶ����ַ����ȡ��ֵ 
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

// ��ָ����Ϊ��Ч    
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
	// 1: �����ں����ﶨ��һ��va_list�͵ı��������������ָ�������ָ��
	va_list arg_ptr;

	// 2: Ȼ����va_start���ʼ�������ն����va_list������
	//    �����ĵڶ��������ǵ�һ���ɱ������ǰһ��������Ҳ�������һ���̶��Ĳ�����
	//    va_start���ص���һ���ɱ������ַ
	va_start(arg_ptr, format);

	std::cout<<"str = "<<format<<std::endl;

//	printf(format);

	unsigned int i = 0 ;
	while(format[i]!='\0')
	{
		if (format[i]=='\\')
		{
			std::cout<<"����\\"<<std::endl;
		}
		i++;
	}

	// va_end������ɱ�����Ļ�ȡ
	va_end(arg_ptr);
*/
}