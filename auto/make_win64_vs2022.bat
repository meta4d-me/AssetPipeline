@echo on

cd ..
if not exist build\. mkdir build
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
cmake ../../assimp -G "Visual Studio 17 2022" -A x64
cd ..
echo\

cd ..
call "./auto/premake5.exe" "vs2022"

pause