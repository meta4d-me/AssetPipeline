@echo on

cd auto
chmod +x ./Premake/Linux/premake5
./Premake/Linux/premake5 --os=linux gmake2