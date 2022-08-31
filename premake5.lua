--------------------------------------------------------------
-- @Description : Makefile of CatDog Engine Tools
--------------------------------------------------------------
workspace("AssetPipeline")
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

	platforms { "x64" }
	filter "platforms:x64"
		architecture "x64"
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
		--path.join(ThirdPartySourcePath, "fbx/2020.2/lib/vs2017/x64/release"),
	}

	links {
		assimpLibName,
		--"libfbxsdk",
	}

	-- copy dll into binary folder automatically.
	local assimpSourcePath = "build\\assimp\\bin"
	local assimpTargetPath = "build\\bin"
	local currentPath = os.getcwd()
	postbuildcommands {
		"cd "..currentPath,
		"{COPYDIR} "..assimpSourcePath.." "..assimpTargetPath,
	}
	postbuildmessage "Copying dependencies..."
end

project("AssetPipeline")
	kind("ConsoleApp")
	language("C++")
	cppdialect("C++latest")
	dependson { "assimp" }

	location("build")
	targetdir("build/bin")

	files {
		path.join("public/**.*"),
		path.join("private/**.*"),
	}
	
	removefiles {
		"private/consumer/fbxconsumer.*",
		"private/producer/fbxproducer.*",
	}
	
	vpaths {
		["Private/*"] = {
			"private/**.*"
		},
		["Public/*"] = { 
			"public/**.*",
		},
	}

	includedirs {
		"public",
		
		-- assimp lib will build a config.h file for the call side to include
		"assimp/include",
		"build/assimp/include",
		
		-- fbx only allow to upload runtime files, not sdk files
		--"fbx/2020.2/include",
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