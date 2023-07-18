@echo on

cd auto
Set USE_CLANG_TOOLSET=1
call "./Premake/Windows/premake5" --os=windows vs2022