CHCP 65001
@ECHO OFF

SET _BUILD_DIR=%~dp0\build
SET _SOURCE_DIR=%~dp0

REM 使用 qt 的 cmake 环境, 不用手动配置工具链, 这里选择自己安装 Qt 的路径
IF NOT EXIST %_BUILD_DIR% ^
CALL "C:\Qt\6.4.1\msvc2022_64\bin\qt-cmake.bat" ^
-B %_BUILD_DIR% -S %_SOURCE_DIR%

CD %_BUILD_DIR%
SET _BUILD_DIR=
SET _SOURCE_DIR=

ECHO "使用命令 cmake --build . --config [Release|Debug] [--verbose] 来编译"
CMD /E:ON /F:ON /k
