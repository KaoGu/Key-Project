:: ����˵��
::
:: �汾��V1.0
:: ���ڣ�2012-07-31
:: ˵����ʵ���ҽ��˵���ɨ�֡�����Ӻ�ɾ��
::
@echo off
title �Ҽ��˵���������
echo.
echo �������Ϊ�����ļ����ļ��е��Ҽ��˵���ӡ��������С��˵�

::goto install
 
:menu
echo.
echo 1.��װ 2.ж�� 3.�˳�
set /p choice=��ѡ�������
echo.
if /i "%choice%" == "1" goto install
if /i "%choice%" == "2" goto remove
if /i "%choice%" == "3" goto exit
echo ������Ч��������ѡ��
goto menu
 
:install
echo ��װ�ļ���
reg add HKEY_CLASSES_ROOT\Folder\shell\cmdkey /ve /t REG_SZ /d "ɨ��" /f > nul
reg add HKEY_CLASSES_ROOT\Folder\shell\cmdkey\command /ve /t REG_SZ /d "D:\project\Key-Project\keyproject\config.bat \"%%1\"" /f > nul
echo ��װ�ļ�
reg add HKEY_CLASSES_ROOT\*\shell\cmdkey /ve /t REG_SZ /d "ɨ��" /f > nul
reg add HKEY_CLASSES_ROOT\*\shell\cmdkey\command /ve /t REG_SZ /d "D:\project\Key-Project\keyproject\config.bat \"%%1\"" /f > nul
echo ��װ��ɣ�
echo.
pause
goto exit
 
:remove
reg delete HKEY_CLASSES_ROOT\Folder\shell\cmdkey\ /f > nul
reg delete HKEY_CLASSES_ROOT\*\shell\cmdkey /f > nul
echo ж����ɣ�
echo.
pause
:exit
:: �ָ����ڱ���
title %comspec%