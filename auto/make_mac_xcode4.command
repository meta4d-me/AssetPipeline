cd ./auto

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
set FBX_SDK_DIR=/usr/local/lib/fbx
set SPEEDTREE_SDK_DIR=/usr/local/lib/speedtree
./auto/Premake/Mac/premake5 xcode4