--------------------------------------------------------------
print("Building generic_producer")

function SetupAssimpLib(assimpLibName, configName)
	libdirs {
		path.join(RootPath, "build/assimp/lib", configName),
	}

	links {
		assimpLibName,
	}
end

-- Assimp producer which used to parse as many kinds of file formats as possible.
project("GenericProducer")
	kind("SharedLib")
	language("C++")
	Platform_SetCppDialect()
	dependson { "AssetPipelineCore", "assimp" }

	location(path.join(RootPath, "build"))

	filter { "configurations:Debug" }
		objdir(path.join(RootPath, "build/obj/Debug"))
		targetdir(path.join(RootPath, "build/bin/Debug"))
		libdirs(path.join(RootPath, "build/bin/Debug"))
		links {
			path.join(RootPath, "build/bin/Debug/AssetPipelineCore")
		}		
	filter { "configurations:Release" }
		objdir(path.join(RootPath, "build/obj/Release"))
		targetdir(path.join(RootPath, "build/bin/Release"))
		libdirs(path.join(RootPath, "build/bin/Release"))
		links {
			path.join(RootPath, "build/bin/Release/AssetPipelineCore")
		}		
	filter {}

	defines {
		"TOOL_BUILD_SHARED"
	}

	files {
		path.join(RootPath, "public/Producers/GenericProducer/**.*"),
		path.join(RootPath, "private/Producers/GenericProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/GenericProducer/**.*"),
			path.join(RootPath, "private/Producers/GenericProducer/**.*"),
		},
	}

	includedirs {
		path.join(RootPath, "public"),
		path.join(RootPath, "private"),
		path.join(RootPath, "external"),
	}
	
	-- assimp
	includedirs {
		-- assimp lib will build a config.h file for the call side to include
		path.join(RootPath, "external/assimp/include"),
		path.join(RootPath, "build/assimp/include"),
	}

	filter { "system:Windows", "configurations:Debug", "action:vs2022" }
			SetupAssimpLib("assimp-vc143-mtd", "Debug")
	filter { "system:Windows", "configurations:Release", "action:vs2022" }
			SetupAssimpLib("assimp-vc143-mt", "Release")
	filter { "system:Windows", "configurations:Debug", "action:vs2019" }
			SetupAssimpLib("assimp-vc142-mtd", "Debug")
	filter { "system:Windows", "configurations:Release", "action:vs2019" }
			SetupAssimpLib("assimp-vc142-mt", "Release")
	filter { "system:Windows", "configurations:Debug", "action:gmake2" }
			SetupAssimpLib("assimp", "Debug")
	filter { "system:Windows", "configurations:Release", "action:gmake2" }
			SetupAssimpLib("assimp", "Release")
	filter{}

	-- Auto copy dlls
	postbuildcommands {
		"cd "..RootPath,
		"{COPYDIR} "..path.join(RootPath, "build\\assimp\\bin").." "..path.join(RootPath, "build\\bin"),
	}
	postbuildmessage "Copying dependencies..."