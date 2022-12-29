--------------------------------------------------------------
-- @Description : Makefile of CatDog Engine Tools
--------------------------------------------------------------

CurrentWorkingPath = os.getcwd()
RootPath = string.sub(CurrentWorkingPath, 0, string.len(CurrentWorkingPath) - string.len("auto") - 1)

--------------------------------------------------------------
-- Define solution
workspace("AssetPipeline")
	location(RootPath)
	architecture "x64"
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
		path.join(RootPath, "auto/*.lua"),
	}

	filter { "system:windows", "action:vs2022" }
		prebuildcommands {
			"cd "..RootPath,
			"make_win64_vs2022.bat",
		}
	filter { "system:windows", "action:vs2019" }
		prebuildcommands {
			"cd "..RootPath,
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
end

dofile("thirdparty.lua")
dofile("core.lua")
dofile("producers.lua")
dofile("consumers.lua")

if not os.istarget("linux") then
	dofile("examples.lua")
end