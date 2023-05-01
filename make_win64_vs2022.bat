@echo on

cd /d %~dp0

if not exist build\. mkdir build
cd build

echo [ ASSIMP ] Start making project...
if not exist assimp\. mkdir assimp
cd assimp
cmake ../../external/assimp -G "Visual Studio 17 2022" -A x64 -DASSIMP_BUILD_ZLIB=ON
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
cd auto
call "./Premake/Windows/premake5" --os=windows vs2022