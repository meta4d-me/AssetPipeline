@echo on

cd auto
Set USE_CLANG_TOOLSET=1
Set CD_PLATFORM=Android
Set CMAKE_TOOLSET_OPTION=-T ClangCL
call "./Premake/Windows/premake5" --os=android vs2022