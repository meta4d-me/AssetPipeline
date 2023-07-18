@echo on

cd auto
Set USE_CLANG_TOOLSET=1
Set CMAKE_TOOLSET_OPTION=-T ClangCL
call "./Premake/Windows/premake5" --os=windows vs2022