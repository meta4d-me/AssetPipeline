--------------------------------------------------------------
-- @Description : Makefile of CatDog Engine Tools
--------------------------------------------------------------

CurrentWorkingPath = os.getcwd()
RootPath = string.sub(CurrentWorkingPath, 0, string.len(CurrentWorkingPath) - string.len("auto") - 1)

ChoosePlatform = os.getenv("CD_PLATFORM") or "Windows"
print("ChoosePlatform = "..ChoosePlatform)

local USE_CLANG_TOOLSET = os.getenv("USE_CLANG_TOOLSET") or "0"
BUILD_ASSIMP = not os.istarget("linux") and USE_CLANG_TOOLSET == "0"
BUILD_FBX = not os.istarget("linux") and USE_CLANG_TOOLSET == "0"
BUILD_TERRAIN = not os.istarget("linux") and USE_CLANG_TOOLSET == "0"
local BUILD_EXAMPLES = not os.istarget("linux") and USE_CLANG_TOOLSET == "0"

--------------------------------------------------------------
-- Define solution
workspace("AssetPipeline")
	location(RootPath)

	filter "system:Windows"
		architecture "x64"
		system("windows")
	filter "system:Android"
		architecture "ARM64"
		androidapilevel(21)
		system("android")
	filter {}

	configurations { "Debug", "Release" }
	filter "configurations:Debug"
		defines { "_DEBUG" }
		symbols("On")
		optimize("Off")
	filter "configurations:Release"
		defines { "NDEBUG" }
		symbols("Off")
		optimize("Full")
	filter {}
	
	filter "system:Windows"
		-- For Windows OS, we want to use latest Windows SDK installed in the PC.
		systemversion("latest")
	filter {}
--------------------------------------------------------------	

--------------------------------------------------------------
-- Define Utility projects
group("Build")
project("AutoMake")
	kind("Utility")
	location(path.join(RootPath, "build"))
	files {
		path.join(RootPath, "auto/**.lua"),
	}

	filter { "system:windows", "action:vs2022" }
		prebuildcommands {
			"cd "..RootPath,
			"Set USE_CLANG_TOOLSET="..USE_CLANG_TOOLSET,
			"make_win64_vs2022.bat",
		}
	filter { "system:windows", "action:vs2019" }
		prebuildcommands {
			"cd "..RootPath,
			"Set USE_CLANG_TOOLSET="..USE_CLANG_TOOLSET,
			"make_win64_vs2019.bat",
		}
	filter { "system:linux", "action:gmake2" }
		prebuildcommands {
			"cd "..RootPath,
			"make_linux_gmake2.sh",
		}
	filter {}
group("")
--------------------------------------------------------------
function Platform_SetCppDialect()
	if os.istarget("linux") then
		cppdialect("c++2a")
	elseif os.istarget("windows") then
		cppdialect("c++latest")
	else
		cppdialect("c++2a")
	end
	
	if USE_CLANG_TOOLSET == "1" then
		toolset("clang")
		cppdialect("c++17")
	end
end

function Platform_LinkSettings()
	filter { "configurations:Debug" }
		objdir(path.join(RootPath, "build/obj/Debug"))
		targetdir(path.join(RootPath, "build/bin/Debug"))
		
		filter { "system:Windows" }
			libdirs(path.join(RootPath, "build/bin/Debug"))
		filter { "system:Android" }
			libdirs(path.join(RootPath, "ARM64/Debug"))
		filter {}			
	filter { "configurations:Release" }
		objdir(path.join(RootPath, "build/obj/Release"))
		targetdir(path.join(RootPath, "build/bin/Release"))
		
		filter { "system:Windows" }
			libdirs(path.join(RootPath, "build/bin/Release"))
		filter { "system:Android" }
			libdirs(path.join(RootPath, "ARM64/Release"))
		filter {}
	filter {}
end

dofile("thirdparty.lua")
dofile("core.lua")
dofile("producers.lua")
dofile("consumers.lua")

if BUILD_EXAMPLES then
	dofile("examples.lua")
end