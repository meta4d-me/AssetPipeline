@echo on

cd /d %~dp0
Set CMAKE_EXE=%~dp0CMake\bin\cmake.exe

cd ..
if not exist build\. mkdir build
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
%CMAKE_EXE% ../../external/assimp -G "Visual Studio 16 2019" -A x64
cd ..
echo\

@echo off
cd ..
if exist "./auto/commercial_sdk_locations.bat" (
    call "./auto/commercial_sdk_locations.bat"
) else (
    echo commercial_sdk_locations does not exist, skipped
)
@echo on
call "./auto/premake5" "vs2019"

pause