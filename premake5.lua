--------------------------------------------------------------
-- @Description : Makefile of CatDog Engine Tools
--------------------------------------------------------------
CommercialSDKConfigs = {}
FBX_SDK_DIR = os.getenv("FBX_SDK_DIR")
if FBX_SDK_DIR and not os.isdir(FBX_SDK_DIR) then
	FBX_SDK_DIR = nil
end

SPEEDTREE_SDK_DIR = os.getenv("SPEEDTREE_SDK_DIR")
if SPEEDTREE_SDK_DIR and not os.isdir(SPEEDTREE_SDK_DIR) then
	SPEEDTREE_SDK_DIR = nil
end

if FBX_SDK_DIR then
	print("Found FBX_SDK_DIR...")
	CommercialSDKConfigs["fbx"] = {}
	CommercialSDKConfigs["fbx"].include = path.join(FBX_SDK_DIR, "include")
	CommercialSDKConfigs["fbx"].lib_dir = path.join(FBX_SDK_DIR, "lib")
	CommercialSDKConfigs["fbx"].lib_names = { "libfbxsdk" }
	CommercialSDKConfigs["fbx"].dll_paths = { path.join(FBX_SDK_DIR, "runtime") }
end

if SPEEDTREE_SDK_DIR then
	print("Found SPEEDTREE_SDK_DIR...")
	CommercialSDKConfigs["st"] = {}
	CommercialSDKConfigs["st"].include = ""
	CommercialSDKConfigs["st"].lib_dir = ""
	CommercialSDKConfigs["st"].lib_names = { }
	CommercialSDKConfigs["st"].dll_paths = { }
end

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

project("AssetPipeline")
	kind("ConsoleApp")
	language("C++")
	cppdialect("C++latest")
	dependson { "assimp" }

	location("build")

	filter { "configurations:Debug" }
		targetdir("build/bin/Debug")
	filter { "configurations:Release" }
		targetdir("build/bin/Release")
	filter {}

	files {
		path.join("public/**.*"),
		path.join("private/**.*"),
	}
	
	allRemoveFiles = {}
	if FBX_SDK_DIR == nil then
		table.insert(allRemoveFiles, "private/consumer/fbxconsumer.*")
		table.insert(allRemoveFiles, "private/producer/fbxproducer.*")
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