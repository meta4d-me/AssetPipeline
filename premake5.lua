--------------------------------------------------------------
-- @Description : Makefile of CatDog Engine Tools
--------------------------------------------------------------

--------------------------------------------------------------
-- All SDK configs
CommercialSDKConfigs = {}
function DefineSDKConfig(sdkDirectoryMacro)
	local sdkDirectory = os.getenv(sdkDirectoryMacro)
	if sdkDirectory and not os.isdir(sdkDirectory) then
		sdkDirectory = nil
	end

	if sdkDirectory then
		print(sdkDirectoryMacro.." found...")
		local sdkConfig = {}
		sdkConfig.include = path.join(sdkDirectory, "include")
		sdkConfig.lib_dir = path.join(sdkDirectory, "lib")
		allLinkLibs = os.matchfiles(sdkConfig.lib_dir.."/*.lib")
		sdkConfig.lib_names = { table.unpack(allLinkLibs) }
		sdkConfig.dll_paths = { path.join(sdkDirectory, "runtime") }
		CommercialSDKConfigs[sdkDirectoryMacro] = sdkConfig
	else
		print(sdkDirectoryMacro.." not found, skip related features...")
	end
end

function CheckSDKExists(sdkDirectoryMacro)
	return CommercialSDKConfigs[sdkDirectoryMacro] ~= nil
end

DefineSDKConfig("FBX_SDK_DIR")
DefineSDKConfig("PHYSX_SDK_DIR")
DefineSDKConfig("SPEEDTREE_SDK_DIR")
--------------------------------------------------------------

--------------------------------------------------------------
-- Define solution
workspace("AssetPipeline")
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
-- Define ThirdParty projects
function DeclareExternalProject(projectName, projectKind, projectPath)
	-- Same with projectName by default
	projectPath = projectPath or projectName

	externalproject(projectName)
		kind(projectKind)
		location(path.join(ThirdPartyProjectPath, projectPath))
end

group "ThirdParty/assimp"
	DeclareExternalProject("assimp", "StaticLib", "build/assimp/code")
		dependson { "zlibstatic" }
	DeclareExternalProject("zlibstatic", "StaticLib", "build/assimp/contrib/zlib")
group ""

function SetupAssimpLib(assimpLibName, configName)
	libdirs {
		path.join("build/assimp/lib", configName),
	}

	links {
		assimpLibName,
	}
end
--------------------------------------------------------------

--------------------------------------------------------------
-- Define lib
project("AssetPipeline")
	kind("StaticLib")
	language("C++")
	cppdialect("C++latest")
	dependson { "assimp" }

	location("build")

	filter { "configurations:Debug" }
		objdir("build/obj/Debug")
		targetdir("build/bin/Debug")
	filter { "configurations:Release" }
		objdir("build/obj/Release")
		targetdir("build/bin/Release")
	filter {}

	--ignoredefaultlibraries {
	--	"LIBCMT"
	--}

	files {
		path.join("public/**.*"),
		path.join("private/**.*"),
	}
	
	allRemoveFiles = {}
	if not CheckSDKExists("FBX_SDK_DIR") then
		table.insert(allRemoveFiles, "private/consumer/fbxconsumer.*")
		table.insert(allRemoveFiles, "private/producer/fbxproducer.*")
	end
	
	if not CheckSDKExists("PHYSX_SDK_DIR") then
		table.insert(allRemoveFiles, "private/consumer/physxconsumer.*")
		table.insert(allRemoveFiles, "private/producer/physxproducer.*")
	end

	removefiles {
		table.unpack(allRemoveFiles)
	}
	
	vpaths {
		["Private/*"] = {
			"private/**.*"
		},
		["Public/*"] = { 
			"public/**.*",
		},
	}

	commercialSDKIncludeDirs = {}
	commercialSDKLibDirs = {}
	commercialSDKLibNames = {}
	for _, config in pairs(CommercialSDKConfigs) do
		table.insert(commercialSDKIncludeDirs, config.include)
		table.insert(commercialSDKLibDirs, config.lib_dir)
		for _, libName in pairs(config.lib_names) do
			table.insert(commercialSDKLibNames, libName)
		end
	end

	includedirs {
		"public",
		"private",
		table.unpack(commercialSDKIncludeDirs)
	}

	libdirs {
		table.unpack(commercialSDKLibDirs),
	}

	links {
		table.unpack(commercialSDKLibNames),
	}

	-- assimp
	includedirs {
		-- assimp lib will build a config.h file for the call side to include
		"assimp/include",
		"build/assimp/include",
	}

	filter { "system:Windows", "configurations:Debug", "action:vs2022" }
			SetupAssimpLib("assimp-vc143-mtd", "Debug")
	filter { "system:Windows", "configurations:Release", "action:vs2022" }
			SetupAssimpLib("assimp-vc143-mt", "Release")
	filter { "system:Windows", "configurations:Debug", "action:vs2019" }
			SetupAssimpLib("assimp-vc142-mtd", "Debug")
	filter { "system:Windows", "configurations:Release", "action:vs2019" }
			SetupAssimpLib("assimp-vc142-mt", "Release")
	filter{}

	-- Auto copy dlls
	local currentWorkingPath = os.getcwd()
	commercialSDKPostCmds = {}
	filter { "configurations:Debug" }
		for _, config in pairs(CommercialSDKConfigs) do
			for _, dllPath in pairs(config.dll_paths) do
				local copyCommand = "{COPYDIR} "..dllPath.." ".."build\\bin\\Debug"
				table.insert(commercialSDKPostCmds, copyCommand)
			end
		end

		postbuildcommands {
			"cd "..currentWorkingPath,
			table.unpack(commercialSDKPostCmds),
			"{COPYDIR} ".."build\\assimp\\bin".." ".."build\\bin",
		}
	filter { "configurations:Release" }
		for _, config in pairs(CommercialSDKConfigs) do
			for _, dllPath in pairs(config.dll_paths) do
				local copyCommand = "{COPYDIR} "..dllPath.." ".."build\\bin\\Release"
				table.insert(commercialSDKPostCmds, copyCommand)
			end
		end

		postbuildcommands {
			"cd "..currentWorkingPath,
			table.unpack(commercialSDKPostCmds),
			"{COPYDIR} ".."build\\assimp\\bin".." ".."build\\bin",
		}
	filter {}
	postbuildmessage "Copying dependencies..."
--------------------------------------------------------------

--------------------------------------------------------------
-- Define examples
function MakeExample(exampleProject)
	if string.contains(exampleProject, "Fbx") and not CheckSDKExists("FBX_SDK_DIR") then
		print("FBX_SDK_DIR not found, Skip example "..exampleProject)
		return
	end

	if string.contains(exampleProject, "Physx") and not CheckSDKExists("PHYSX_SDK_DIR") then
		print("PHYSX_SDK_DIR not found, Skip example "..exampleProject)
		return
	end

	print("Making example : "..exampleProject)
	project(exampleProject)
		kind("ConsoleApp")
		language("C++")
		cppdialect("C++latest")
		dependson { "AssetPipeline" }
		
		location("build")
		
		filter { "configurations:Debug" }
			objdir("build/obj/Debug")
			targetdir("build/bin/Debug")
		filter { "configurations:Release" }
			objdir("build/obj/Release")
			targetdir("build/bin/Release")
		filter {}
		
		files {
			"examples/"..exampleProject.."/**.*"
		}
		
		includedirs {
			"public",
			"private",
		}

		filter { "configurations:Debug" }
			libdirs {
				"build/bin/Debug/"
			}
		filter { "configurations:Release" }
			libdirs {
				"build/bin/Release/"
			}
		filter {}

		links {
			"AssetPipeline"
		}
end
		
group "Examples"
exampleProjectFolders = os.matchdirs("examples/*")
for _, exampleProject in pairs(exampleProjectFolders) do
	MakeExample(path.getbasename(exampleProject))
end
group ""
--------------------------------------------------------------