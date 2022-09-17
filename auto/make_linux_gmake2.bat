@echo on

cd /d %~dp0

cd ..
if not exist build\. mkdir build
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
cmake ../../assimp -G "Unix Makefiles"
cd ..
echo\

cd ..
call "./auto/Linux/Premake/premake5" "gmake2"

pause