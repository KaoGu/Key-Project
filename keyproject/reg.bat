:: 更新说明
::
:: 版本：V1.0
:: 日期：2012-07-31
:: 说明：实现右健菜单“扫街”的添加和删除
::
@echo off
title 右键菜单打开命令行
echo.
echo 本程序可为任意文件及文件夹的右键菜单添加“打开命令行”菜单

::goto install
 
:menu
echo.
echo 1.安装 2.卸载 3.退出
set /p choice=请选择操作：
echo.
if /i "%choice%" == "1" goto install
if /i "%choice%" == "2" goto remove
if /i "%choice%" == "3" goto exit
echo 输入无效，请重新选择！
goto menu
 
:install
echo 安装文件夹
reg add HKEY_CLASSES_ROOT\Folder\shell\cmdkey /ve /t REG_SZ /d "扫街" /f > nul
reg add HKEY_CLASSES_ROOT\Folder\shell\cmdkey\command /ve /t REG_SZ /d "D:\project\Key-Project\keyproject\config.bat \"%%1\"" /f > nul
echo 安装文件
reg add HKEY_CLASSES_ROOT\*\shell\cmdkey /ve /t REG_SZ /d "扫街" /f > nul
reg add HKEY_CLASSES_ROOT\*\shell\cmdkey\command /ve /t REG_SZ /d "D:\project\Key-Project\keyproject\config.bat \"%%1\"" /f > nul
echo 安装完成！
echo.
pause
goto exit
 
:remove
reg delete HKEY_CLASSES_ROOT\Folder\shell\cmdkey\ /f > nul
reg delete HKEY_CLASSES_ROOT\*\shell\cmdkey /f > nul
echo 卸载完成！
echo.
pause
:exit
:: 恢复窗口标题
title %comspec%