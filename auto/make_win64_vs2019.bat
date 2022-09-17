@echo on

cd /d %~dp0

cd ..
if not exist build\. mkdir build
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
cmake ../../assimp -G "Visual Studio 16 2019" -A x64
cd ..
echo\

cd ..
call "./auto/Premake/Windows/premake5" "vs2019"

pause