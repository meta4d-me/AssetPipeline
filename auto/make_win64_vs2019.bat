@echo off

rem generate
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
%CMAKE_EXE% ../../assimp -G %CMAKE_IDE_FULL_NAME% -A x64
cd ..
echo\

pause