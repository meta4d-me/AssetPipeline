@echo on

cd /d %~dp0
Set CMAKE_EXE=%~dp0CMake\bin\cmake.exe

cd ..
if not exist build\. mkdir build
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
%CMAKE_EXE% ../../assimp -G "Visual Studio 16 2019" -A x64
cd ..
echo\

cd ..
call "./auto/premake5" "vs2019"

pause