@echo on

cd /d %~dp0

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
call "./auto/commercial_sdk_locations.bat"
call "./auto/Premake/Windows/premake5" "vs2022"

pause