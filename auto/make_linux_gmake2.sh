#!/bin/sh

cd auto

cd ..
mkdir -p build
cd build

echo [ ASSIMP ] Start making project...
mkdir -p assimp
cd assimp
cmake ../../assimp -G "Unix Makefiles"
cd ..
echo\

cd ..
set FBX_SDK_DIR=/usr/local/lib/fbx
set SPEEDTREE_SDK_DIR=/usr/local/lib/speedtree
chmod +x ./auto/Premake/Linux/premake5
./auto/Premake/Linux/premake5 gmake2